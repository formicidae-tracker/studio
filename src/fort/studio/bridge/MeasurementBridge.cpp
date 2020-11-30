#include "MeasurementBridge.hpp"

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/studio/Format.hpp>

#include <QtConcurrent>
#include <QDebug>

#include <fort/myrmidon/utils/Defer.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>

#include "ExperimentBridge.hpp"
#include "UniverseBridge.hpp"

MeasurementBridge::MeasurementBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_typeModel( new QStandardItemModel (this) ) {

	connect(d_typeModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &MeasurementBridge::onTypeItemChanged);

}

MeasurementBridge::~MeasurementBridge() {
}

void MeasurementBridge::initialize(ExperimentBridge * experiment) {
}

void MeasurementBridge::tearDownExperiment() {
	d_typeModel->clear();
	d_typeModel->setHorizontalHeaderLabels({tr("Name"),tr("TypeID")});
}

void MeasurementBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}

	for (const auto & [mtID,type] : d_experiment->MeasurementTypes()) {
		d_typeModel->appendRow(buildType(type));
	}
}

QAbstractItemModel * MeasurementBridge::typeModel() const {
	return d_typeModel;
}

bool MeasurementBridge::setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
                                       fmp::MeasurementType::ID mtID,
                                       QPointF start,
                                       QPointF end) {
	if ( !d_experiment ) {
		return false;
	}

	auto tddURI = tcu->Frame().ParentURI();

	Eigen::Vector2d startFromTag = tcu->ImageToTag() * Eigen::Vector2d(start.x(),start.y());
	Eigen::Vector2d endFromTag = tcu->ImageToTag() * Eigen::Vector2d(end.x(),end.y());
	auto m = std::make_shared<fmp::Measurement>(tcu->URI(),
	                                            mtID,
	                                            startFromTag,
	                                            endFromTag,
	                                            tcu->TagSizePx());

	try {
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::SetMeasurement('"
		         << m->URI().c_str() << "')";
		d_experiment->SetMeasurement(m);
	} catch (const std::exception & e ) {
		qCritical() << "Could not set measurement '"
		            << m->URI().c_str() << "': " << e.what();
		return false;
	}


	qInfo() << "Set measurement '" << m->URI().c_str() << "'";
	setModified(true);
	emit measurementModified(m);
	return true;
}

void MeasurementBridge::deleteMeasurement(const fmp::Measurement::ConstPtr & m) {
	if ( isActive() == false || m == nullptr ) {
		return;
	}

	try {
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::DeleteMeasurement('"
		         << m->URI().c_str() << "')";
		d_experiment->DeleteMeasurement(m->URI());
	} catch (const std::exception & e) {
		qWarning() << "Could not delete measurement '" << m->URI().c_str()
		           << "':" << e.what();
		return;
	}

	qInfo() << "Deleted measurement '" << m->URI().c_str() << "'";
	setModified(true);
	emit measurementDeleted(m);
}



void MeasurementBridge::setMeasurementType(quint32 mtID, const QString & name) {
	if ( !d_experiment ) {
		return;
	}
	try {
		auto fi = d_experiment->MeasurementTypes().find(mtID);
		if ( fi == d_experiment->MeasurementTypes().end() ) {
			qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::CreateMeasurement('" << name << "')";
			auto type = d_experiment->CreateMeasurementType(ToStdString(name));
			mtID = type->MTID();
			d_typeModel->appendRow(buildType(type));
		} else {
			auto items = d_typeModel->findItems(QString::number(mtID),Qt::MatchExactly,0);
			if ( items.size() != 1 ) {
				throw std::logic_error("Internal type model error");
			}
			qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::MeasurementType::SetName('" << name << "')";
			fi->second->SetName(name.toUtf8().data());
			d_typeModel->item(items[0]->row(),1)->setText(name);
		}
	} catch ( const std::exception & e) {
		qCritical() << "Could not set MeasurementType " << mtID << " to '" << name << "': " << e.what();
	}

	qInfo() << "Set MeasurementType " << mtID
	        << " name to '" << name << "'";
	setModified(true);
	emit measurementTypeModified(mtID,name);
}

void MeasurementBridge::deleteMeasurementType(const QModelIndex & index) {
	auto item = d_typeModel->itemFromIndex(index);
	if ( item == NULL ) {
		qWarning() << "Could not delete measurement type at index " << index;
		return;
	}
	auto mtype = item->data().value<fmp::MeasurementType::Ptr>();
	deleteMeasurementType(mtype->MTID());
}

void MeasurementBridge::deleteMeasurementType(quint32 mtID) {
	if ( !d_experiment ) {
		return;
	}

	try {
		auto items = d_typeModel->findItems(QString::number(mtID),Qt::MatchExactly,1);
		if ( items.size() != 1 ) {
			throw std::logic_error("Internal type model error");
		}
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::Experiment::DeleteMeasurementType("
		         << mtID << ")";
		d_experiment->DeleteMeasurementType(mtID);
		d_typeModel->removeRows(items[0]->row(),1);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete MeasurementType " << mtID << ": " << e.what();
		return;
	}

	qInfo() << "Deleted MeasurementType " << mtID;
	setModified(true);
	emit measurementTypeDeleted(mtID);
}

QList<QStandardItem *> MeasurementBridge::buildType(const fmp::MeasurementType::Ptr & type) const {
	auto mtID =new QStandardItem(QString::number(type->MTID()));
	mtID->setEditable(false);
	mtID->setData(QVariant::fromValue(type));
	auto name = new QStandardItem(type->Name().c_str());
	auto icon = Conversion::iconFromFM(fmp::Palette::Default().At(type->MTID()));
	name->setIcon(icon);
	name->setData(QVariant::fromValue(type));
	name->setEditable(true);
	return {name,mtID};
}

void MeasurementBridge::onTypeItemChanged(QStandardItem * item) {
	if (item->column() != 0) {
		qDebug() << "[MeasurmentBridge]: Ignoring measurement type item change for column " << item->column();
		return;
	}

	auto type = item->data().value<fmp::MeasurementType::Ptr>();
	std::string newName = ToStdString(item->text());
	if ( newName == type->Name() ) {
		qDebug() << "[MeasurementBridge]:  Ignoring MEasurementType item change '"
		         << item->text() << "': it is still the same";
		return;
	}

	try {
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::MeasurementType::SetName('"
		         << item->text() << "')";
		type->SetName(newName);
	} catch( const std::exception & e) {
		qCritical() << "Could not change measurement type " << type->MTID()
		            << ":'" << type->Name().c_str()
		            <<"' to '" << item->text()
		            << "': " << e.what();
		item->setText(type->Name().c_str());
		return;
	}
	qInfo() << "Set MeasurementType " << type->MTID()
	        << " to '" << item->text() << "'";
	setModified(true);
	emit measurementTypeModified(type->MTID(),type->Name().c_str());
}


fmp::Measurement::ConstPtr MeasurementBridge::measurementForCloseUp(const std::string & tcuURI,
                                                                    fmp::MeasurementTypeID type) {
	if ( !d_experiment ) {
		return fmp::Measurement::ConstPtr();
	}
	auto tcufi = d_experiment->Measurements().find(tcuURI);
	if ( tcufi == d_experiment->Measurements().end()) {
		return fmp::Measurement::ConstPtr();
	}
	auto fi = tcufi->second.find(type);
	if ( fi == tcufi->second.end() ) {
		return fmp::Measurement::ConstPtr();
	}
	return fi->second;
}
