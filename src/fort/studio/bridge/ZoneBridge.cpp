#include "ZoneBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <fort/studio/Format.hpp>

const int ZoneBridge::TypeRole = Qt::UserRole+1;
const int ZoneBridge::DataRole = Qt::UserRole+2;

ZoneBridge::ZoneBridge(QObject * parent)
	: Bridge(parent)
	, d_spaceModel(new QStandardItemModel(this)) {
	qRegisterMetaType<fmp::Space::Ptr>();
	qRegisterMetaType<fmp::Zone::Ptr>();
	qRegisterMetaType<fmp::Zone::Definition::Ptr>();
}


void ZoneBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;

	rebuildSpaces();
	emit activated(!d_experiment == false);
}

QAbstractItemModel * ZoneBridge::spaceModel() const {
	return d_spaceModel;
}

void ZoneBridge::rebuildSpaces() {
	d_spaceModel->clear();
	d_spaceModel->setHorizontalHeaderLabels({tr("ID"),tr("Name"),tr("Size")});
	if ( !d_experiment ) {
		return;
	}
	for ( const auto & [spaceID,space] : d_experiment->Spaces() ) {
		d_spaceModel->appendRow(buildSpace(space));
	}
}


void ZoneBridge::onTrackingDataDirectoryChange(const QString & uri) {
}


void ZoneBridge::addItemAtIndex(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if (item == nullptr ) {
		return;
	}
	//selects the first column
	item = item->parent()->child(item->row(),0);
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
	item = item->parent()->child(item->row());
	switch ( item->data(TypeRole).toInt() ) {
	case SpaceType:
		return;
	case ZoneType:
		removeZone(item);
		break;
	case DefinitionType:
		removeDefinition(item->parent());
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
		         << ToQString(start) << ","
		         << ToQString(end)
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

}


void ZoneBridge::addZone(QStandardItem * spaceRootItem) {
	fmp::Zone::Ptr z;
	try {
		qDebug() << "[ZoneBridge]: Calling fort::myrmidon::priv::Space::CreateZone('new-zone')";
		z = spaceRootItem->data(DataRole).value<fmp::Space::Ptr>()->CreateZone(ToStdString(tr("new-zone")));
		qDebug() << "[ZoneBridge]: Calling fort::myrmidon::priv::Zone::AddDefinition({},nullptr,nullptr)";
		z->AddDefinition({},fm::Time::ConstPtr(),fm::Time::ConstPtr());
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
	qInfo() << "Created zone " << spaceRootItem->data(Qt::DisplayRole).toInt()
	        << "." << z->ZoneID() << "'" << ToQString(z->Name()) << "'";
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

}

void ZoneBridge::removeDefinition(QStandardItem * definitionItem) {
	auto zoneItem = definitionItem->parent();
	auto z = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	if (!z) {
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
	res.push_back(new QStandardItem(ToQString(definition->Start())));
	res.push_back(new QStandardItem(ToQString(definition->End())));
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
	item = item->parent()->child(item->row());
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
	item = item->parent()->child(item->row());
	switch(item->data(TypeRole).toInt()) {
	case SpaceType:
		return false;
	case DefinitionType:
	case ZoneType:
		return true;
	}
}
