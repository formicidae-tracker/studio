#include "IdentifierBridge.hpp"

#include <QDebug>
#include <QItemSelection>

#include <fort/studio/Format.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include <fort/myrmidon/priv/Identifier.hpp>

#include "ExperimentBridge.hpp"
#include "GlobalPropertyBridge.hpp"

IdentifierBridge::IdentifierBridge(QObject * parent)

	: GlobalBridge(parent)
	, d_model(new QStandardItemModel(this)) {

	qRegisterMetaType<fmp::Ant::ConstPtr>();
	qRegisterMetaType<fmp::Ant::Ptr>();
	qRegisterMetaType<fmp::Identification::ConstPtr>();


	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &IdentifierBridge::onIdentificationItemChanged);
}

IdentifierBridge::~IdentifierBridge() {}

void IdentifierBridge::initialize(ExperimentBridge * experiment) {
	connect(experiment->globalProperties(),
	        &GlobalPropertyBridge::tagSizeChanged,
	        this,
	        &IdentifierBridge::onDefaultTagSizeChanged);
}

void IdentifierBridge::tearDownExperiment() {
	if ( isActive() == true ) {
		d_experiment->Identifier()
			->SetAntPositionUpdateCallback([](const fmp::Identification::Ptr & i,
			                                  const std::vector<fmp::AntPoseEstimateConstPtr> & estimations) {
			                               });
	}
}

void IdentifierBridge::setUpExperiment() {
	rebuildModels();
	if ( isActive() == true ) {
		d_experiment->Identifier()
			->SetAntPositionUpdateCallback([this](const fmp::Identification::Ptr & identification,
			                                      const std::vector<fmp::AntPoseEstimateConstPtr> & estimations) {
				                               onAntPositionUpdate(identification,estimations);
			                               });
	}
}

fmp::Identification::Ptr IdentifierBridge::addIdentification(fm::Ant::ID antID,
                                                             fmp::TagID tagID,
                                                             const fm::Time & start,
                                                             const fm::Time & end) {

	if ( !d_experiment) {
		qWarning() << "Not Adding Identification to Ant " << fmp::Ant::FormatID(antID).c_str();
		return fmp::Identification::Ptr();
	}

	fmp::Identification::Ptr identification;
	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifider::AddIdentification( "
		         << fmp::Ant::FormatID(antID).c_str()
		         << "," << fmp::FormatTagID(tagID).c_str()
		         <<  "," << ToQString(start)
		         << "," << ToQString(end) << ")";
		identification = fmp::Identifier::AddIdentification(d_experiment->Identifier(),
		                                                    antID,tagID,start,end);
	} catch (const std::exception & e) {
		qCritical() << "Could not create Identification " << fmp::Ant::FormatID(antID).c_str()
		            << " ↤ " << fmp::FormatTagID(tagID).c_str()
		            << " [" << ToQString(start)
		            << ";" << ToQString(end)
		            << "]: " << e.what();
		return fmp::Identification::Ptr();
	}

	qInfo() << "Added Identification " << ToQString(identification);
	d_model->insertRow(d_model->rowCount(),
	                   buildIdentification(identification));

	emit identificationCreated(identification);
	setModified(true);
	return identification;
}

void IdentifierBridge::deleteIdentification(const fmp::Identification::ConstPtr & identification) {
	auto identificationItem = findIdentification(identification);
	if ( !d_experiment
	     || identificationItem == NULL ) {
		qWarning() << "Not deleting Identification "
		           << ToQString(identification);
		return ;
	}

	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::DeleteIdentification("
		         << ToQString(identification) << ")";
		d_experiment->Identifier()->DeleteIdentification(identificationItem->data().value<fmp::Identification::Ptr>());
	} catch (const std::exception & e ) {
		std::ostringstream os;
		os << *identification;
		qCritical() << "Could not delete identification " << os.str().c_str()
		            << ": " <<  e.what();
		return;
	}

	qInfo() << "Deleted identification " << ToQString(identification);

	d_model->removeRows(identificationItem->row(),1);
	setModified(true);
	emit identificationDeleted(identification);



}



static void setSizeItem(QStandardItem * item,
                        double defaultSize,
                        const fmp::Identification::ConstPtr & identification) {
	if ( identification->UseDefaultTagSize() == true ) {
		item->setText(QString::number(defaultSize,'f',2));
		item->setData(QColor(0,0,255),Qt::ForegroundRole);
	} else {
		item->setText(QString::number(identification->TagSize(),'f',2));
		item->setData(QVariant(),Qt::ForegroundRole);
	}
}

QList<QStandardItem*> IdentifierBridge::buildIdentification(const fmp::Identification::Ptr & identification) {
	auto data = QVariant::fromValue(identification);

	auto tagID = new QStandardItem(fm::FormatTagID(identification->TagValue()).c_str());
	auto antID = new QStandardItem(identification->Target()->FormattedID().c_str());
	auto start = new QStandardItem(ToQString(identification->Start()));
	auto end = new QStandardItem(ToQString(identification->End()));
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




fmp::Identification::ConstPtr IdentifierBridge::identify(fmp::TagID tagID,
                                                         const fm::Time & time) const {
	if ( !d_experiment ) {
		return fmp::Identification::ConstPtr();
	}
	return d_experiment->CIdentifier().Identify(tagID,time);
}


bool IdentifierBridge::freeRangeContaining(fm::Time & start,
                                           fm::Time & end,
                                           fmp::TagID tagID, const fm::Time & time) const {
	if ( !d_experiment ) {
		return false;
	}
	return d_experiment->CIdentifier().FreeRangeContaining(start,end,tagID,time);
}


void IdentifierBridge::rebuildModels() {

	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("TagID"),tr("AntID"),tr("Start"),tr("End"),tr("Size"),tr("Poses")});

	if ( isActive() == false ) {
		return;
	}

	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		for (const auto & identification : ant->Identifications() ) {
			d_model->insertRow(d_model->rowCount(),buildIdentification(identification));
		}
	}
}


QStandardItem * IdentifierBridge::findIdentification(const fmp::Identification::ConstPtr & identification) const {
	auto items = d_model->findItems(fm::FormatTagID(identification->TagValue()).c_str(),
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
		d_model->item(item->row(),POSES_COLUMN)->setText(QString::number(estimations.size()));
	}

	emit identificationAntPositionModified(identification);
}


static fm::Time parseTime(const QString & timeStr,
                          const std::string & prefix ) {
	if ( timeStr.isEmpty() == true  || timeStr == (prefix + "∞").c_str() ) {
		if ( prefix == "-" ) {
			return fm::Time::SinceEver();
		} else {
			return fm::Time::Forever();
		}
	}
	return  fm::Time::Parse(timeStr.toUtf8().constData());
}


#define onRangeItemChanged(RangeName,prefix) do {	  \
	auto identification = item->data().value<fmp::Identification::Ptr>(); \
	try { \
		auto RangeName ## Time = parseTime(item->text(),prefix); \
		if ( (RangeName ## Time == identification->RangeName()) == false) { \
			identification->Set## RangeName (RangeName ## Time); \
			setModified(true); \
			qInfo() << ToQString(*identification) << #RangeName " time to " << ToQString(identification->RangeName()); \
			emit identificationRangeModified(identification); \
		} \
	} catch ( const std::exception & e) { \
		qCritical() << "Could not set " #RangeName " time to " << item->text() << ": " << e.what(); \
	} \
	QSignalBlocker blocker(d_model); \
	item->setText(ToQString(identification->RangeName())); \
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
	if ( item->text().isEmpty() == true ) {
		if ( identification->UseDefaultTagSize() == false ) {
			identification->SetTagSize(fmp::Identification::DEFAULT_TAG_SIZE);
			setModified(true);
			qInfo() << "Set identification " << ToQString(*identification) << " to default tag size";
			emit identificationSizeModified(identification);
		}
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
	QSignalBlocker blocker(d_model);
	setSizeItem(item,d_experiment->DefaultTagSize(),identification);
}

QAbstractItemModel * IdentifierBridge::model() const {
	return d_model;
}


void IdentifierBridge::onDefaultTagSizeChanged(double defaultTagSize) {
	QSignalBlocker blocker(d_model);
	for(int i = 0; i < d_model->rowCount(); ++i) {
		auto item = d_model->item(i,SIZE_COLUMN);
		setSizeItem(item,defaultTagSize,item->data().value<fmp::Identification::Ptr>());
	}

}

fmp::Identification::ConstPtr IdentifierBridge::identificationForIndex(const QModelIndex & index) const {
	auto item  = d_model->itemFromIndex(index);
	if ( item == nullptr ) {
		return nullptr;
	}
	return item->data().value<fmp::Identification::Ptr>();
}

std::vector<fm::Ant::ID> IdentifierBridge::unidentifiedAntAt(const fm::Time & time) const {
	std::vector<fm::Ant::ID> res;
	for ( const auto & [antID,ant] : d_experiment->CIdentifier().CAnts() ) {
		const auto & identifications = ant->CIdentifications();
		if ( std::find_if(identifications.begin(),
		                  identifications.end(),
		                  [&time](const fmp::Identification::ConstPtr & ident) {
			                  return ident->IsValid(time);
		                  }) != identifications.cend() ) {
			continue;
		}
		res.push_back(antID);
	}
	return res;
}
