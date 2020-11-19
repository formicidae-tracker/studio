#include "IdentifierBridge.hpp"

#include <QDebug>
#include <QItemSelection>

#include <fort/studio/Format.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include <fort/myrmidon/priv/Identifier.hpp>

#include "SelectedAntBridge.hpp"

IdentifierBridge::IdentifierBridge(QObject * parent)
	: Bridge(parent)
	, d_antModel(new QStandardItemModel(this))
	, d_identificationModel(new QStandardItemModel(this))
	, d_numberSoloAnt(0)
	, d_numberHiddenAnt(0)
	, d_selectedAnt(new SelectedAntBridge(this)) {

	qRegisterMetaType<fmp::Ant::ConstPtr>();
	qRegisterMetaType<fmp::Ant::Ptr>();
	qRegisterMetaType<fmp::Identification::ConstPtr>();
	qRegisterMetaType<fmp::Ant::DisplayState>();
	qRegisterMetaType<fm::Color>();

	connect(d_antModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &IdentifierBridge::onAntItemChanged);

	connect(d_identificationModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &IdentifierBridge::onIdentificationItemChanged);


}

IdentifierBridge::~IdentifierBridge() {}

bool IdentifierBridge::isActive() const {
	return d_experiment.get() != NULL;
}


QAbstractItemModel * IdentifierBridge::antModel() const {
	return d_antModel;
}




void IdentifierBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[IdentifierBridge]: setting new experiment";
	d_numberSoloAnt = 0;
	d_numberHiddenAnt = 0;
	d_selectedAnt->setAnt(fmp::Ant::Ptr());

	setModified(false);
	if ( d_experiment ) {
		d_experiment->Identifier()
			->SetAntPositionUpdateCallback([](const fmp::Identification::Ptr & i,
			                                  const std::vector<fmp::AntPoseEstimateConstPtr> & estimations) {
			                               });
	}

	d_experiment = experiment;
	rebuildModels();

	if ( !d_experiment ) {
		emit activated(false);
		emit numberSoloAntChanged(d_numberSoloAnt);
		emit numberHiddenAntChanged(d_numberHiddenAnt);
	} else {
		d_experiment->Identifier()
			->SetAntPositionUpdateCallback([this](const fmp::Identification::Ptr & identification,
			                                      const std::vector<fmp::AntPoseEstimateConstPtr> & estimations) {
				                               onAntPositionUpdate(identification,estimations);
			                               });
		emit activated(true);
		emit numberSoloAntChanged(d_numberSoloAnt);
		emit numberHiddenAntChanged(d_numberHiddenAnt);
	}
}

fmp::Ant::Ptr IdentifierBridge::createAnt() {
	if ( !d_experiment ) {
		return fmp::Ant::Ptr();
	}
	fmp::Ant::Ptr ant;
	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Experiment::CreateAnt()";
		ant = d_experiment->CreateAnt();
	} catch ( const std::exception & e) {
		qCritical() << "Could not create Ant: " << e.what();
		return fmp::Ant::Ptr();
	}

	qInfo() << "Created new Ant" << ant->FormattedID().c_str();

	d_antModel->insertRow(d_antModel->rowCount(),buildAnt(ant));

	setModified(true);
	emit antCreated(ant);
	return ant;
}

void IdentifierBridge::deleteAnt(fm::Ant::ID antID) {
	auto item = findAnt(antID);
	if ( !d_experiment || item == NULL) {
		qWarning() << "Not removing Ant " << fmp::Ant::FormatID(antID).c_str();
		return;
	}

	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::DeleteAnt("
		         << fmp::Ant::FormatID(antID).c_str() << ")";
		d_experiment->Identifier()->DeleteAnt(antID);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete Ant '" <<  fmp::Ant::FormatID(antID).c_str()
		            << "': " << e.what();
		return;
	}

	qInfo() << "Deleted Ant " << fmp::Ant::FormatID(antID).c_str();

	d_antModel->removeRows(item->row(),1);
	setModified(true);
	emit antDeleted(antID);
}


fmp::Identification::Ptr IdentifierBridge::addIdentification(fm::Ant::ID antID,
                                                             fmp::TagID tagID,
                                                             const fm::Time::ConstPtr & start,
                                                             const fm::Time::ConstPtr & end) {

	auto item = findAnt(antID);

	if ( !d_experiment || item == NULL ) {
		qWarning() << "Not Adding Identification to Ant " << fmp::Ant::FormatID(antID).c_str();
		return fmp::Identification::Ptr();
	}

	fmp::Identification::Ptr identification;
	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifider::AddIdentification( "
		         << fmp::Ant::FormatID(antID).c_str()
		         << "," << fmp::FormatTagID(tagID).c_str()
		         <<  "," << ToQString(start,"-")
		         << "," << ToQString(end,"+") << ")";
		identification = fmp::Identifier::AddIdentification(d_experiment->Identifier(),
		                                                    antID,tagID,start,end);
	} catch (const std::exception & e) {
		qCritical() << "Could not create Identification " << fmp::Ant::FormatID(antID).c_str()
		            << " ↤ " << fmp::FormatTagID(tagID).c_str()
		            << " [" << ToQString(start,"-")
		            << ";" << ToQString(end,"+")
		            << "]: " << e.what();
		return fmp::Identification::Ptr();
	}

	qInfo() << "Added Identification " << ToQString(identification);
	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));

	d_identificationModel->insertRow(d_identificationModel->rowCount(),
	                                 buildIdentification(identification));

	emit identificationCreated(identification);
	setModified(true);
	return identification;
}

void IdentifierBridge::deleteIdentification(const fmp::Identification::Ptr & identification) {
	auto antItem = findAnt(identification->Target()->AntID());
	auto identificationItem = findIdentification(identification);
	if ( !d_experiment
	     || antItem == NULL
	     || identificationItem == NULL ) {
		qWarning() << "Not deleting Identification "
		           << ToQString(identification);
		return ;
	}

	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::DeleteIdentification("
		         << ToQString(identification) << ")";
		d_experiment->Identifier()->DeleteIdentification(identification);
	} catch (const std::exception & e ) {
		std::ostringstream os;
		os << *identification;
		qCritical() << "Could not delete identification " << os.str().c_str()
		            << ": " <<  e.what();
		return;
	}

	qInfo() << "Deleted identification " << ToQString(identification);

	antItem->setText(formatAntName(antItem->data().value<fmp::Ant::Ptr>()));
	d_identificationModel->removeRows(identificationItem->row(),1);
	setModified(true);
	emit identificationDeleted(identification);



}

QString IdentifierBridge::formatAntName(const fmp::Ant::Ptr & ant) {
	QString res = ant->FormattedID().c_str();
	if ( ant->Identifications().empty() ) {
		return res + " <no-tags>";
	}
	std::set<fmp::TagID> tags;
	for ( const auto & i : ant->Identifications() ) {
		tags.insert(i->TagValue());
	}
	QString prefix = " ↤ {";
	for ( const auto & t : tags ) {
		res += prefix + fmp::FormatTagID(t).c_str();
		prefix = ",";
	}
	return res + "}";
}

QList<QStandardItem*> IdentifierBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto data = QVariant::fromValue(ant);
	auto label = new QStandardItem(formatAntName(ant));
	label->setEditable(false);
	label->setData(data);
	label->setData(antDisplayColor(ant),Qt::DecorationRole);

	auto hidden = new QStandardItem("");
	hidden->setCheckable(true);
	hidden->setData(data);
	hidden->setData(tr("Hide this ant in visualization"),Qt::ToolTipRole);
	hidden->setData(tr("Do not show this Ant in visualization."),Qt::WhatsThisRole);
	auto solo = new QStandardItem("");
	solo->setCheckable(true);
	solo->setData(data);
	solo->setData(tr("Solo this ant in visualization"),Qt::ToolTipRole);
	solo->setData(tr("When some Ant are Solo-ed, only this Ant would be displayed in visualization."),Qt::WhatsThisRole);
	switch(ant->DisplayStatus()) {
	case fmp::Ant::DisplayState::VISIBLE:
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::HIDDEN:
		++d_numberHiddenAnt;
		hidden->setCheckState(Qt::Checked);
		solo->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
		++d_numberSoloAnt;
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Checked);
		break;
	}

	return {label,hidden,solo};
}

static void setSizeItem(QStandardItem * item,
                        double defaultSize,
                        const fmp::Identification::ConstPtr & identification) {
	if ( identification->UseDefaultTagSize() == true ) {
		item->setText(QString::number(defaultSize));
		item->setData(QColor(0,0,255),Qt::ForegroundRole);
	} else {
		item->setText(QString::number(identification->TagSize()));
		item->setData(QVariant(),Qt::ForegroundRole);
	}
}

QList<QStandardItem*> IdentifierBridge::buildIdentification(const fmp::Identification::Ptr & identification) {
	auto data = QVariant::fromValue(identification);

	auto tagID = new QStandardItem(fm::FormatTagID(identification->TagValue()).c_str());
	auto antID = new QStandardItem(identification->Target()->FormattedID().c_str());
	auto start = new QStandardItem(ToQString(identification->Start(),"-"));
	auto end = new QStandardItem(ToQString(identification->End(),"+"));
	auto size = new QStandardItem();
	setSizeItem(size,d_experiment->DefaultTagSize(),identification);
	std::vector<fmp::AntPoseEstimateConstPtr> estimations;
	d_experiment->Identifier()->QueryAntPoseEstimate(estimations,identification);
	auto poses = new QStandardItem(QString::number(estimations.size()));

	QList<QStandardItem*> res = {tagID,antID,start,end,size,poses};
	for ( const auto & item : res ) {
		item->setEditable(false);
		item->setData(data);
	}
	start->setEditable(true);
	end->setEditable(true);
	size->setEditable(true);


	return res;
}


QIcon IdentifierBridge::antDisplayColor(const fmp::Ant::Ptr & ant) {
	auto c = ant->DisplayColor();
	return Conversion::iconFromFM(c);
}



QStandardItem * IdentifierBridge::findAnt(fm::Ant::ID antID) const {
	auto items = d_antModel->findItems(fmp::Ant::FormatID(antID).c_str(), Qt::MatchStartsWith);
	if ( items.size() != 1 ) {
		qDebug() << "Could not find Ant " << fmp::Ant::FormatID(antID).c_str();
		return NULL;
	}
	return items[0];
}

void IdentifierBridge::setAntDisplayState(QStandardItem * hideItem,
                                          QStandardItem * soloItem,
                                          const fmp::Ant::Ptr & ant,
                                          fmp::Ant::DisplayState ds) {
	auto oldDs = ant->DisplayStatus();
	if ( oldDs == ds ) {
		return;
	}
	ant->SetDisplayStatus(ds);
	switch(oldDs) {
	case fm::Ant::DisplayState::HIDDEN:
		--d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		break;
	case fm::Ant::DisplayState::SOLO:
		--d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		break;
	case fm::Ant::DisplayState::VISIBLE:
		break;
	}

	switch(ds) {
	case fmp::Ant::DisplayState::VISIBLE:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to VISIBLE";
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::HIDDEN:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to HIDDEN";
		++d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		hideItem->setCheckState(Qt::Checked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to SOLO";
		++d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Checked);
		break;
	}
	setModified(true);
	emit antDisplayChanged(ant->AntID(),ant->DisplayColor(),ant->DisplayStatus());
}

void IdentifierBridge::onAntItemChanged(QStandardItem * item) {
	if ( item->column() < HIDE_COLUMN || item->column() > SOLO_COLUMN ) {
		return;
	}

	auto ant = item->data().value<fmp::Ant::Ptr>();
	auto hideItem = d_antModel->item(item->row(),HIDE_COLUMN);
	auto soloItem = d_antModel->item(item->row(),SOLO_COLUMN);
	switch ( item->column() ) {
	case HIDE_COLUMN:
		if ( item->checkState() == Qt::Checked ){
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::HIDDEN);
		} else if (soloItem->checkState() == Qt::Unchecked ) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
		}
		break;
	case SOLO_COLUMN:
		if ( item->checkState() == Qt::Checked ) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::SOLO);
		} else if ( hideItem->checkState() == Qt::Unchecked) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
		}
		break;
	}

}

void IdentifierBridge::onIdentificationItemChanged(QStandardItem * item) {
	switch ( item->column() ) {
	case START_COLUMN:
		onStartItemChanged(item);
		break;
	case END_COLUMN:
		onEndItemChanged(item);
		break;
	case SIZE_COLUMN:
		onSizeItemChanged(item);
		break;
	default:
		return;
	}
}



void IdentifierBridge::selectAnt(const QModelIndex & index) {
	if ( index.isValid() == false
	     || index.column() != 0 ) {
		qDebug() << "[IdentifierBridge]: Wrong column " <<  index.column() << " for selection";
		return;
	}

	auto ant = d_antModel->itemFromIndex(index)->data().value<fmp::Ant::Ptr>();
	d_selectedAnt->setAnt(ant);
}


void IdentifierBridge::doOnSelection(const QItemSelection & selection,
                                     const std::function<void (const fmp::Ant::Ptr & ant,
                                                               QStandardItem * item)> & toDo) {
	for ( const auto & index : selection.indexes() ) {
		if ( index.isValid() == false || index.column() != 0 ) {
			continue;
		}
		auto item = d_antModel->itemFromIndex(index);
		auto ant = item->data().value<fmp::Ant::Ptr>();
		if ( !ant ) {
			continue;
		}
		toDo(ant,item);
	}
}


void IdentifierBridge::setAntDisplayColor(const QItemSelection & selection,
                                          const QColor & color) {
	if ( color.isValid() == false ) {
		return;
	}
	doOnSelection(selection,
	              [this,&color](const fmp::Ant::Ptr & ant,
	                            QStandardItem * item) {
		              qInfo() << "Setting Display Color of Ant " << ant->FormattedID().c_str()
		                      << " to " << color;
		              ant->SetDisplayColor({color.red(),color.green(),color.blue()});

		              item->setData(antDisplayColor(ant),Qt::DecorationRole);

		              setModified(true);
		              emit antDisplayChanged(ant->AntID(),ant->DisplayColor(),ant->DisplayStatus());
	              });
}


void IdentifierBridge::deleteSelection(const QItemSelection & selection) {
	std::set<fmp::Identification::Ptr> toDeleteIdentifications;
	std::set<fm::Ant::ID> toDeleteAntID;
	doOnSelection(selection,
	              [&,this](const fmp::Ant::Ptr & ant,
	                     QStandardItem *) {
		              for( const auto & i : ant->Identifications() ) {
			              toDeleteIdentifications.insert(i);
		              }
		              toDeleteAntID.insert(ant->AntID());
	              });

	for ( const auto & i : toDeleteIdentifications ) {
		deleteIdentification(i);
	}
	for ( const auto & AID : toDeleteAntID ) {
		deleteAnt(AID);
	}
}

quint32 IdentifierBridge::numberHiddenAnt() const {
	return d_numberHiddenAnt;
}

quint32 IdentifierBridge::numberSoloAnt() const {
	return d_numberSoloAnt;
}


void IdentifierBridge::showAll() {
	for(size_t i = 0; i < d_antModel->rowCount(); ++i) {
		auto hideItem = d_antModel->itemFromIndex(d_antModel->index(i,HIDE_COLUMN));
		auto soloItem = d_antModel->itemFromIndex(d_antModel->index(i,SOLO_COLUMN));
		auto ant = hideItem->data().value<fmp::Ant::Ptr>();
		setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
	}
}

void IdentifierBridge::unsoloAll() {
	for(size_t i = 0; i < d_antModel->rowCount(); ++i) {
		auto hideItem = d_antModel->itemFromIndex(d_antModel->index(i,HIDE_COLUMN));
		auto soloItem = d_antModel->itemFromIndex(d_antModel->index(i,SOLO_COLUMN));
		auto ant = hideItem->data().value<fmp::Ant::Ptr>();
		if (ant->DisplayStatus() != fmp::Ant::DisplayState::SOLO ) {
			continue;
		}
		setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
	}
}

fmp::Identification::ConstPtr IdentifierBridge::identify(fmp::TagID tagID,
                                                         const fm::Time & time) const {
	if ( !d_experiment ) {
		return fmp::Identification::ConstPtr();
	}
	return d_experiment->CIdentifier().Identify(tagID,time);
}


bool IdentifierBridge::freeRangeContaining(fm::Time::ConstPtr & start,
                                           fm::Time::ConstPtr & end,
                                           fmp::TagID tagID, const fm::Time & time) const {
	if ( !d_experiment ) {
		return false;
	}
	return d_experiment->CIdentifier().FreeRangeContaining(start,end,tagID,time);
}

SelectedAntBridge * IdentifierBridge::selectedAnt() const {
	return d_selectedAnt;
}

fmp::Ant::ConstPtr IdentifierBridge::ant(fm::Ant::ID aID) const {
	if ( !d_experiment == true ) {
		return fmp::Ant::ConstPtr();
	}
	const auto & ants = d_experiment->CIdentifier().CAnts();
	auto fi = ants.find(aID);
	if ( fi == ants.cend() ) {
		return fmp::Ant::ConstPtr();
	}
	return fi->second;
}


void IdentifierBridge::rebuildModels() {
	d_antModel->clear();
	d_antModel->setHorizontalHeaderLabels({tr("AntID"),tr("H"),tr("S")});

	d_identificationModel->clear();
	d_identificationModel->setHorizontalHeaderLabels({tr("TagID"),tr("AntID"),tr("Start"),tr("End"),tr("Size"),tr("Poses")});

	if ( !d_experiment ) {
		return;
	}


	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		d_antModel->insertRow(d_antModel->rowCount(),buildAnt(ant));
		for (const auto & identification : ant->Identifications() ) {
			d_identificationModel->insertRow(d_identificationModel->rowCount(),buildIdentification(identification));
		}
	}
}


QStandardItem * IdentifierBridge::findIdentification(const fmp::Identification::ConstPtr & identification) const {
	auto items = d_identificationModel->findItems(fm::FormatTagID(identification->TagValue()).c_str(),
	                                              Qt::MatchExactly,
	                                              TAG_ID_COLUMN);
	for ( const auto & item : items ) {
		if ( item->data().value<fmp::Identification::Ptr>() == identification ) {
			return item;
		}
	}
	return nullptr;
}

void IdentifierBridge::onAntPositionUpdate(const fmp::Identification::ConstPtr & identification,
                                           const std::vector<fmp::AntPoseEstimateConstPtr> & estimations) {

	auto item = findIdentification(identification);
	if ( item != nullptr ) {
		d_identificationModel->item(item->row(),POSES_COLUMN)->setText(QString::number(estimations.size()));
	}

	emit identificationAntPositionModified(identification);
}


static fm::Time::ConstPtr parseTime(const QString & timeStr) {
	if ( timeStr.isEmpty() == true || timeStr == "-∞" || timeStr == "+∞" ) {
		return nullptr;
	}
	return  std::make_shared<fm::Time>(fm::Time::Parse(timeStr.toUtf8().constData()));
}

static bool timePtrEqual(const fm::Time::ConstPtr & a,
                         const fm::Time::ConstPtr & b ) {
	if (!a) {
		return !b;
	}
	if ( !b ) {
		return false;
	};

	return *a == *b;
}

#define onRangeItemChanged(RangeName,prefix) do {	  \
	auto identification = item->data().value<fmp::Identification::Ptr>(); \
	try { \
		auto RangeName ## Time = parseTime(item->text()); \
		if ( timePtrEqual(RangeName ## Time,identification->RangeName()) == false) { \
			identification->Set## RangeName (RangeName ## Time); \
			setModified(true); \
			qInfo() << ToQString(*identification) << #RangeName " time to " << ToQString(identification->RangeName(),prefix); \
			emit identificationRangeModified(identification); \
		} \
	} catch ( const std::exception & e) { \
		qCritical() << "Could not set " #RangeName " time " << item->text() << ": " << e.what(); \
	} \
	QSignalBlocker blocker(d_identificationModel); \
	item->setText(ToQString(identification->RangeName(),prefix));\
	} while(0)

void IdentifierBridge::onStartItemChanged(QStandardItem * item) {
	onRangeItemChanged(Start,"-");
}

void IdentifierBridge::onEndItemChanged(QStandardItem * item) {
	onRangeItemChanged(End,"+");
}

#undef onRangeItemChanged

void IdentifierBridge::onSizeItemChanged(QStandardItem * item) {
	auto identification = item->data().value<fmp::Identification::Ptr>();
	if ( item->text().isEmpty() == true
	     && identification->UseDefaultTagSize() == false ) {

		identification->SetTagSize(fmp::Identification::DEFAULT_TAG_SIZE);
		setModified(true);
		qInfo() << "Set identification " << ToQString(*identification) << " to default tag size";
		emit identificationSizeModified(identification);
	} else {
		bool ok = false;
		double tagSize = item->text().toDouble(&ok);
		if ( ok == true
		     && tagSize != identification->TagSize() ) {
			identification->SetTagSize(tagSize);
			setModified(true);
			qInfo() << "Set identification " << ToQString(*identification) << " tag size to " << tagSize;
			emit identificationSizeModified(identification);
		} else {
			qCritical() << "Could not parse tag size " << item->text();
		}
	}
	QSignalBlocker blocker(d_identificationModel);
	setSizeItem(item,d_experiment->DefaultTagSize(),identification);
}

QAbstractItemModel * IdentifierBridge::identificationsModel() const {
	return d_identificationModel;
}
