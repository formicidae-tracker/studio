#include "MeasurementBridge.hpp"

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/studio/Format.hpp>

#include <QtConcurrent>
#include <QDebug>

#include <fort/myrmidon/utils/Defer.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>




MeasurementBridge::MeasurementBridge(QObject * parent)
	: Bridge(parent)
	, d_tcuModel( new QStandardItemModel (this) )
	, d_typeModel( new QStandardItemModel (this) ) {

	connect(d_typeModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &MeasurementBridge::onTypeItemChanged);

}

MeasurementBridge::~MeasurementBridge() {
}

bool MeasurementBridge::isActive() const {
	return d_experiment.get() != NULL;
}


QAbstractItemModel * MeasurementBridge::tagCloseUpModel() const {
	return d_tcuModel;
}

QAbstractItemModel * MeasurementBridge::measurementTypeModel() const {
	return d_typeModel;
}

void MeasurementBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[MeasurementBridge]: setting new experiment";
	setModified(false);
	d_typeModel->clear();
	d_typeModel->setHorizontalHeaderLabels({tr("Name"),tr("TypeID")});
	d_experiment = experiment;
	if ( !d_experiment ) {
		emit activated(false);
		return;
	}

	for (const auto & [mtID,type] : d_experiment->MeasurementTypes()) {
		d_typeModel->appendRow(buildType(type));
	}

	emit activated(true);
	loadAllTagCloseUps();

}

void MeasurementBridge::onTDDAdded(const fmp::TrackingDataDirectoryPtr & tdd) {
	if ( !d_experiment  ) {
		return;
	}
	qDebug() << "[MeasurementBrdige]: new TDD "<< tdd->URI().c_str();
	loadTagCloseUps(tdd);
}

void MeasurementBridge::onTDDDeleted(const QString & tddURI) {
	if ( !d_experiment  ) {
		return;
	}
	qDebug() << "[MeasurementBrdige]: Removing TDD "<< tddURI;
	clearTddTCUs(tddURI.toUtf8().constData());
}

void MeasurementBridge::loadTagCloseUps(const fmp::TrackingDataDirectory::Ptr & tdd) {
	for ( const auto & tcu : tdd->TagCloseUps() ) {
		addOneTCU(tdd->URI(),tcu);
	}

}
void MeasurementBridge::loadAllTagCloseUps() {
	clearAllTCUs();
	if ( !d_experiment
	     || d_experiment->Family() == fort::tags::Family::Undefined ) {
		return;
	}

	for ( const auto & [tddURI,tdd] : d_experiment->TrackingDataDirectories() ) {
		loadTagCloseUps(tdd);
	}
}


QList<QStandardItem*> MeasurementBridge::buildTag(fmp::TagID tagID) const {
	auto tagItem = new QStandardItem(QString("tags/%1").arg(fmp::FormatTagID(tagID).c_str()));
	tagItem->setEditable(false);
	tagItem->setData(tagID,Qt::UserRole+1);
	tagItem->setData(tagID,Qt::UserRole+2);
	QList<QStandardItem*> res = {tagItem};
	for ( size_t i = 0; i < 2; ++i ) {
		auto dummyItem = new QStandardItem("");
		dummyItem->setEditable(false);
		dummyItem->setData(tagID,Qt::UserRole+1);
		res.push_back(dummyItem);
	}
	return res;
}

QList<QStandardItem*> MeasurementBridge::buildTCU(const fmp::TagCloseUp::ConstPtr & tcu) {
	if ( !d_experiment ) {
		return {};
	}

	auto tcuItem = new QStandardItem(tcu->URI().c_str());
	tcuItem->setEditable(false);
	tcuItem->setData(QVariant::fromValue(tcu),Qt::UserRole+1);
	tcuItem->setData(qulonglong(tcu->Frame().FrameID()),Qt::UserRole+2);
	size_t mCount = countMeasurementsForTCU(tcu->URI());

	auto measurementCounts = new QStandardItem(QString("%1").arg(mCount));
	measurementCounts->setEditable(false);
	measurementCounts->setData(QVariant::fromValue(tcu),Qt::UserRole+1);
	d_counts.insert(std::make_pair(tcu->URI(),measurementCounts));
	return {tcuItem,measurementCounts};
}


void MeasurementBridge::addOneTCU(const std::string & tddURI,
                                  const fmp::TagCloseUp::ConstPtr & tcu) {
	auto target = tcu->TagValue();

	QString tagPath = QString("tags/%1");
	tagPath = tagPath.arg(fmp::FormatTagID(target).c_str());

	auto items = d_tcuModel->findItems(tagPath);
	QStandardItem * tagItem = NULL;
	if ( items.size() == 0 ) {
		auto tagItems = buildTag(target);
		tagItem = tagItems[0];
		d_tcuModel->invisibleRootItem()->appendRow(tagItems);
	} else {
		tagItem = items[0];
	}

	tagItem->appendRow(buildTCU(tcu));

	auto fi = d_closeups.insert(std::make_pair(tddURI,CloseUpByPath()));
	fi.first->second.insert(std::make_pair(tcu->URI(),tcu));
}

void MeasurementBridge::clearTddTCUs(const std::string & tddURI) {
	auto fi = d_closeups.find(tddURI);
	if ( fi == d_closeups.end() ){
		return;
	}
	for ( const auto & [uri,tcu] : fi->second ){
		auto items = d_tcuModel->findItems(uri.c_str());
		for(const auto item : items) {
			auto index = item->index();
			d_tcuModel->removeRows(index.row(),1,index.parent());
		}
		d_counts.erase(uri);
	}
	d_closeups.erase(fi);
}

void MeasurementBridge::clearAllTCUs() {
	d_tcuModel->clear();
	d_tcuModel->setHorizontalHeaderLabels({tr("URI"),tr("Nb Measurements"),tr("")});
	d_closeups.clear();
	d_counts.clear();
}

bool MeasurementBridge::setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
                                       fmp::MeasurementType::ID mtID,
                                       QPointF start,
                                       QPointF end) {
	if ( !d_experiment ) {
		return false;
	}

	auto tddURI = tcu->Frame().ParentURI();
	auto fi = d_closeups.find(tddURI);
	auto ci = d_counts.find(tcu->URI());
	if ( ci == d_counts.end()
	     || fi == d_closeups.end()
	     || fi->second.count(tcu->URI()) == 0 ) {
		qWarning() << "Not setting measurement: unknown '" << tcu->URI().c_str() << "'";
		return false;
	}

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
	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcu->URI())));

	setModified(true);
	emit measurementModified(m);
	return true;
}

void MeasurementBridge::deleteMeasurement(const std::string & mURI) {
	if ( !d_experiment ) {
		return;
	}

	auto [tddURI,frameID,tagID,mtID] = fmp::Measurement::DecomposeURI(mURI);

	auto tcuURI = fs::path(mURI).parent_path().parent_path().generic_string();
	auto ci = d_counts.find(tcuURI);
	if ( ci == d_counts.end() ) {
		qWarning() << "Unknown measurement '" << mURI.c_str() << "'";
		return;
	}

	try {
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::DeleteMeasurement('"
		         << mURI.c_str() << "')";
		d_experiment->DeleteMeasurement(mURI);
	} catch (const std::exception & e) {
		qWarning() << "Could not delete measurement '" << mURI.c_str()
		           << "':" << e.what();
		return;
	}
	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcuURI)));

	qInfo() << "Deleted measurement '" << mURI.c_str() << "'";
	setModified(true);
	emit measurementDeleted(ToQString(tcuURI),mtID);
}


size_t MeasurementBridge::countMeasurementsForTCU(const std::string & tcuPath) const {
	if ( !d_experiment ){
		return 0;
	}
	auto mi = d_experiment->Measurements().find(tcuPath);
	if ( mi == d_experiment->Measurements().end() ) {
		return 0;
	}
	return mi->second.size();
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



fmp::TagCloseUp::ConstPtr MeasurementBridge::fromTagCloseUpModelIndex(const QModelIndex & index) {
	if ( index.parent().isValid() == false ){
		return fmp::TagCloseUp::ConstPtr();
	}

	return d_tcuModel->itemFromIndex(index)->data(Qt::UserRole+1).value<fmp::TagCloseUp::ConstPtr>();
}

fmp::Measurement::ConstPtr MeasurementBridge::measurement(const std::string & tcuURI,
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


void MeasurementBridge::queryTagCloseUp(QVector<fmp::TagCloseUp::ConstPtr> & tcus,
                                        const fmp::IdentificationConstPtr & identification) {
	auto items = d_tcuModel->findItems(QString("tags/%1").arg(fmp::FormatTagID(identification->TagValue()).c_str()));
	if ( items.isEmpty() == true ) {
		return;
	}
	tcus.reserve(tcus.size() + items[0]->rowCount());

	for ( size_t i =  0; i < items[0]->rowCount(); ++i) {
		auto tcu = items[0]->child(i)->data().value<fmp::TagCloseUp::ConstPtr>();
		if ( !tcu) {
			continue;
		}
		if ( identification->IsValid(tcu->Frame().Time()) == true ) {
			tcus.push_back(tcu);
		}
	}

}
