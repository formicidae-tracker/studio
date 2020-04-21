#include "ZoneBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <fort/studio/Format.hpp>

const int ZoneBridge::TypeRole = Qt::UserRole+1;
const int ZoneBridge::DataRole = Qt::UserRole+2;

Q_DECLARE_METATYPE(ZoneBridge::FullFrame)

ZoneBridge::ZoneBridge(QObject * parent)
	: Bridge(parent)
	, d_spaceModel(new QStandardItemModel(this))
	, d_fullFrameModel( new QStandardItemModel(this))
	, d_zoneModel( new QStandardItemModel(this)) {

	qRegisterMetaType<fmp::Space::Ptr>();
	qRegisterMetaType<fmp::Zone::Ptr>();
	qRegisterMetaType<fmp::Zone::Definition::Ptr>();
	qRegisterMetaType<ZoneBridge::FullFrame>();

	connect(d_spaceModel,&QStandardItemModel::itemChanged,
	        this,&ZoneBridge::onItemChanged);
}


void ZoneBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
	d_selectedSpace.reset();


	setModified(false);
	rebuildSpaces();

	emit activated(!d_experiment == false);
}

bool ZoneBridge::isActive() const{
	return !d_experiment == false;
}


QAbstractItemModel * ZoneBridge::spaceModel() const {
	return d_spaceModel;
}

QAbstractItemModel * ZoneBridge::zonesModel() const {
	return d_zoneModel;
}

QAbstractItemModel * ZoneBridge::fullFrameModel() const {
	return d_fullFrameModel;
}


void ZoneBridge::rebuildSpaces() {
	d_spaceModel->clear();
	d_spaceModel->setHorizontalHeaderLabels({tr("ID"),tr("Name"),tr("Size")});
	d_selectedSpace.reset();
	rebuildFullFrameModel();
	rebuildZoneModel();

	if ( !d_experiment ) {
		return;
	}
	for ( const auto & [spaceID,space] : d_experiment->Spaces() ) {
		d_spaceModel->appendRow(buildSpace(space));
	}
}

void ZoneBridge::onTrackingDataDirectoryChange(const QString & uri) {
	if ( !d_experiment == true || !d_selectedSpace == true) {
		return;
	}
	auto located = d_experiment->LocateTrackingDataDirectory(ToStdString(uri));
	if ( !located.first == true || located.first != d_selectedSpace ) {
		return;
	}
	rebuildFullFrameModel();
}


QStandardItem * ZoneBridge::getSibling(QStandardItem * item, int column) {
	return d_spaceModel->itemFromIndex(d_spaceModel->sibling(item->row(),column,item->index()));
}

void ZoneBridge::addItemAtIndex(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if (item == nullptr ) {
		return;
	}
	//selects the first column
	item = getSibling(item,0);
	switch ( item->data(TypeRole).toInt() ) {
	case SpaceType:
		addZone(item);
		break;
	case ZoneType:
		addDefinition(item);
		break;
	case DefinitionType:
		addDefinition(item->parent());
		break;
	default:
		break;
	}
}

void ZoneBridge::removeItemAtIndex(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if (item == nullptr ) {
		return;
	}
	//selects the first column
	item = getSibling(item,0);
	switch ( item->data(TypeRole).toInt() ) {
	case SpaceType:
		return;
	case ZoneType:
		removeZone(item);
		break;
	case DefinitionType:
		removeDefinition(item);
		break;
	default:
		break;
	}
}


void ZoneBridge::addDefinition(QStandardItem * zoneRootItem) {
	fmp::Zone::Definition::Ptr definition;
	auto z = zoneRootItem->data(DataRole).value<fmp::Zone::Ptr>();
	fm::Time::ConstPtr start,end;
	if ( !z == true || z->NextFreeTimeRegion(start,end) == false ) {
		return;
	}

	try {
		qDebug() << "[ZoneBridge]: Calling fmp::Zone::AddDefinition({},"
		         << ToQString(start,"-") << ","
		         << ToQString(end,"+")
		         << ")";
		z->AddDefinition(std::make_shared<fmp::Zone::Geometry>(std::vector<fmp::Shape::ConstPtr>()),
		                 start,end);
	} catch ( const std::exception & e) {
		qCritical() << "Coul not create definition: " << e.what();
		return;
	}
	qInfo() << "Created new definition for zone " << ToQString(z->Name());
	setModified(true);
	zoneRootItem->removeRows(0,zoneRootItem->rowCount());
	for ( const auto & definition : z->Definitions() ) {
		zoneRootItem->appendRow(buildDefinition(definition));
	}
	getSibling(zoneRootItem,2)->setText(QString::number(zoneRootItem->rowCount()));
}


void ZoneBridge::addZone(QStandardItem * spaceRootItem) {
	fmp::Zone::Ptr z;
	auto space = spaceRootItem->data(DataRole).value<fmp::Space::Ptr>();
	if ( !space == true ) {
		return;
	}
	try {
		qDebug() << "[ZoneBridge]: Calling fort::myrmidon::priv::Space::CreateZone('new-zone')";
		z = space->CreateZone(ToStdString(tr("new-zone")));
		qDebug() << "[ZoneBridge]: Calling fort::myrmidon::priv::Zone::AddDefinition({},nullptr,nullptr)";
		z->AddDefinition({},
		                 fm::Time::ConstPtr(),
		                 fm::Time::ConstPtr());
	} catch ( const std::exception & e) {
		qCritical() << "Could not create Zone: " << e.what();
		return;
	}
	setModified(true);
	int insertionRow = 0;
	for ( ; insertionRow < spaceRootItem->rowCount(); ++insertionRow) {
		if ( spaceRootItem->child(insertionRow)->data(Qt::DisplayRole).toInt() > z->ZoneID() ) {
			break;
		};
	}
	spaceRootItem->insertRow(insertionRow,buildZone(z));
	getSibling(spaceRootItem,2)->setText(QString::number(spaceRootItem->rowCount()));
	qInfo() << "Created zone " << spaceRootItem->data(Qt::DisplayRole).toInt()
	        << "." << z->ZoneID() << "'" << ToQString(z->Name()) << "'";
	if ( space == d_selectedSpace ) {
		rebuildZoneModel();
	}
}

void ZoneBridge::removeZone(QStandardItem * zoneItem) {
	auto spaceItem = zoneItem->parent();
	auto zone = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	auto space = spaceItem->data(DataRole).value<fmp::Space::Ptr>();
	if ( !space || !zone ) {
		return;
	}
	try {
		qDebug() << "[ZoneBridge]: Calling fmp::Space::DeleteZone("
		         << zone->ZoneID()
		         << ")";
		space->DeleteZone(zone->ZoneID());
	} catch ( const std::exception & e) {
		qCritical() << "Could not delete zone: " << e.what();
		return;
	}
	setModified(true);
	spaceItem->removeRows(zoneItem->row(),1);
	getSibling(spaceItem,2)->setText(QString::number(spaceItem->rowCount()));
	if (space == d_selectedSpace ) {
		rebuildZoneModel();
	}
}

void ZoneBridge::removeDefinition(QStandardItem * definitionItem) {
	auto zoneItem = definitionItem->parent();
	auto z = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	if (!z == true ) {
		return;
	}
	try {
		qDebug() << "[ZoneBridge]: Calling fmp::Zone::EraseDefinition("
		         << definitionItem->row() << ")";
		z->EraseDefinition(definitionItem->row());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not remove definition: " << e.what();
		return;
	}
	setModified(true);
	zoneItem->removeRows(definitionItem->row(),1);
	getSibling(zoneItem,2)->setText(QString::number(zoneItem->rowCount()));
	qInfo() << "Removed Zone definition";
}



QList<QStandardItem*> ZoneBridge::buildSpace(const fmp::Space::Ptr & space) const {
	auto typeData = QVariant(SpaceType);
	auto data = QVariant::fromValue(space);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(QString::number(space->SpaceID())));
	res.push_back(new QStandardItem(ToQString(space->Name())));
	res.push_back(new QStandardItem(QString::number(space->Zones().size())));
	for ( const auto & i : res ) {
		i->setEditable(false);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	for ( const auto & [zID,zone] : space->Zones() ) {
		res[0]->appendRow(buildZone(zone));
	}
	return res;
}

QList<QStandardItem*> ZoneBridge::buildZone(const fmp::Zone::Ptr & zone) const {
	auto typeData = QVariant(ZoneType);
	auto data = QVariant::fromValue(zone);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(QString::number(zone->ZoneID())));
	res.push_back(new QStandardItem(ToQString(zone->Name())));
	res.push_back(new QStandardItem(QString::number(zone->Definitions().size())));
	for ( const auto & i : res ) {
		i->setEditable(false);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	res[1]->setEditable(true);
	for ( const auto & definition : zone->Definitions() ) {
		res[0]->appendRow(buildDefinition(definition));
	}
	return res;
}

QList<QStandardItem*> ZoneBridge::buildDefinition(const fmp::Zone::Definition::Ptr & definition) const {
	auto typeData = QVariant(DefinitionType);
	auto data = QVariant::fromValue(definition);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(ToQString(definition->Start(),"-")));
	res.push_back(new QStandardItem(ToQString(definition->End(),"+")));
	res.push_back(new QStandardItem(QString::number(definition->GetGeometry()->Shapes().size())));
	for ( const auto & i : res ) {
		i->setEditable(true);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	res[2]->setEditable(false);
	return res;
}


bool ZoneBridge::canAddItemAt(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return false;
	}
	item = getSibling(item,0);
	fm::Time::ConstPtr start,end;
	switch(item->data(TypeRole).toInt()) {
	case SpaceType:
		return true;
	case DefinitionType:
		item = item->parent();
	case ZoneType:
		return item->data(DataRole).value<fmp::Zone::Ptr>()->NextFreeTimeRegion(start,end);
	}
}

bool ZoneBridge::canRemoveItemAt(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return false;
	}
	item = getSibling(item,0);
	switch(item->data(TypeRole).toInt()) {
	case SpaceType:
		return false;
	case DefinitionType:
	case ZoneType:
		return true;
	}
}


void ZoneBridge::onItemChanged(QStandardItem * item) {
	if (item->data(TypeRole).toInt() == ZoneType && item->column() == 1 ) {
		changeZoneName(item);
		return;
	}

	if (item->data(TypeRole).toInt() == DefinitionType) {
		if ( item->column() == 0 ) {
			changeDefinitionTime(item,true);
			return;
		}
		if ( item->column() == 1 ) {
			changeDefinitionTime(item,false);
			return;
		}
	}
}

void ZoneBridge::changeZoneName(QStandardItem * zoneNameItem) {
	auto z = zoneNameItem->data(DataRole).value<fmp::Zone::Ptr>();
	std::string newName = ToStdString(zoneNameItem->text());
	if ( !z == true || z->Name() == newName ) {
		return;
	}
	auto oldName = ToQString(z->Name());
	try {
		qDebug() << "[ZoneBridge]: Calling fmp::Zone::SetName("
		         << zoneNameItem->text()
		         << ")";
		z->SetName(newName);
	} catch ( const std::exception & e) {
		qCritical() << "Could not set Zone '" << oldName
		            << "' name to " << zoneNameItem->text()
		            << ": " << e.what();
		zoneNameItem->setText(oldName);
		return;
	}

	setModified(true);
	qInfo() << "Changed zone name '" << oldName
	        << "' to '" << zoneNameItem->text() << "'";

	auto space = getSibling(zoneNameItem,0)->parent()->data(DataRole).value<fmp::Space::Ptr>();
	if ( !space == false && space == d_selectedSpace ) {
		rebuildZoneModel();
	}
}

void ZoneBridge::changeDefinitionTime(QStandardItem * definitionTimeItem, bool start) {
	auto d = definitionTimeItem->data(DataRole).value<fmp::Zone::Definition::Ptr>();
	if ( !d == true  ) {
		return;
	}

	auto prefix = start == true ? "-" : "+";
	auto oldTime = start == true ? d->Start() : d->End();
	auto oldTimeStr = ToQString(oldTime,prefix);
	if ( oldTimeStr  == definitionTimeItem->text() ) {
		return;
	}

	fm::Time::ConstPtr newTime;
	if ( definitionTimeItem->text().isEmpty() == false ) {
		try {
			newTime = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(definitionTimeItem->text())));
		} catch ( const std::exception & e ) {
			qCritical() << "Could not parse time " << definitionTimeItem->text();
			definitionTimeItem->setText(oldTimeStr);
			return;
		}
	}
	auto newTimeStr = ToQString(newTime,prefix);

	try {
		if ( start == true ) {
			qDebug() << "[ZoneBridge]: Calling fmp::Zone::Definition::SetStart("
			         << newTimeStr
			         << ")";
			d->SetStart(newTime);
		} else {
			qDebug() << "[ZoneBridge]: Calling fmp::Zone::Definition::SetEnd("
			         << newTimeStr
			         << ")";
			d->SetEnd(newTime);
		}
	} catch ( std::exception & e ) {
		qCritical() << "Could not set Zone::Definition start/end to " << newTimeStr
		            << ": " << e.what();
		definitionTimeItem->setText(oldTimeStr);
		return;
	}

	setModified(true);
	definitionTimeItem->setText(newTimeStr);
	qInfo() << "Set Zone::Definition start to " << newTimeStr;
}


void ZoneBridge::activateItem(QModelIndex index) {
	fmp::Space::Ptr newSpace;
	if ( index.isValid() != false ){
		index = d_spaceModel->sibling(index.row(),0,index);

		while(index.parent().isValid() == true ) {
			index = index.parent();
		}
		newSpace = d_spaceModel->itemFromIndex(index)->data(DataRole).value<fmp::Space::Ptr>();
	}

	if ( newSpace == d_selectedSpace ) {
		return;
	}
	d_selectedSpace = newSpace;
	rebuildFullFrameModel();
	rebuildZoneModel();
}


void ZoneBridge::rebuildFullFrameModel() {
	d_fullFrameModel->clear();
	d_fullFrameModel->setHorizontalHeaderLabels({tr("URI")});

	if ( !d_selectedSpace == true ) {
		return;
	}

	for ( const auto & tdd : d_selectedSpace->TrackingDataDirectories() ) {
		for ( const auto & [ref,path] : tdd->FullFrames() ) {
			auto item = new QStandardItem(ref.URI().c_str());
			item->setEditable(false);
			item->setData(QVariant::fromValue(FullFrame{ref,path.c_str()}));
			d_fullFrameModel->appendRow({item});
		}

	}

}

std::pair<bool,ZoneBridge::FullFrame> ZoneBridge::fullFrameAtIndex(const QModelIndex & index) const {
	auto item = d_fullFrameModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return std::make_pair(false,FullFrame());
	}

	return std::make_pair(true,item->data().value<FullFrame>());
}

void ZoneBridge::rebuildZoneModel() {
	d_zoneModel->clear();

	if ( !d_selectedSpace == true ) {
		return;
	}

	for ( const auto [zID,zone] : d_selectedSpace->Zones() ) {
		auto c = fmp::Palette::Default().At(zID);
		auto item = new QStandardItem(ToQString(zone->Name()));
		item->setEditable(false);
		item->setIcon(Conversion::iconFromFM(c));
		item->setData(Conversion::colorFromFM(c));
		item->setData(zID,Qt::UserRole+1);
		d_zoneModel->appendRow({item});
	}

}
