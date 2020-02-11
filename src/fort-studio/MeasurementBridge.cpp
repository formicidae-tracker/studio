#include "MeasurementBridge.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QtConcurrent>
#include <QDebug>


TagCloseUpLoader::TagCloseUpLoader(const fmp::TrackingDataDirectoryConstPtr & tdd,
                                   fort::tags::Family f,
                                   uint8_t threshold,
                                   QObject * parent)
	: QObject(parent)
	, d_tddURI(tdd->URI())
	, d_futureWatcher(new QFutureWatcher<fmp::TagCloseUp::List>(this))
	, d_lister(tdd->TagCloseUpLister(f,threshold)) {
	connect(d_futureWatcher,
	        SIGNAL(resultReadyAt(int)),
	        this,
	        SLOT(onResultReady(int)));
}


void TagCloseUpLoader::waitForFinished() {
	d_futureWatcher->waitForFinished();
}

fmp::TagCloseUp::List TagCloseUpLoader::Load(const fmp::TagCloseUp::Lister::Loader & l) {
	return l();
}


void TagCloseUpLoader::cancel() {
	d_futureWatcher->cancel();
}

void TagCloseUpLoader::start() {
	auto loaders = d_lister->PrepareLoaders();
	d_futureWatcher->setFuture(QtConcurrent::mapped(loaders,TagCloseUpLoader::Load));
}

void TagCloseUpLoader::onResultReady(int index) {
	auto tags = d_futureWatcher->resultAt(index);
	for (auto & t : tags) {
		emit newTagCloseUp(d_tddURI,d_lister->Family(),d_lister->Threshold(),t);
	}
}


MeasurementBridge::MeasurementBridge(QObject * parent)
	: QObject(parent)
	, d_tcuModel( new QStandardItemModel (this) )
	, d_typeModel( new QStandardItemModel (this) )
	, d_experiment(NULL) {

	connect(d_typeModel,
	        SIGNAL(itemChanged(QStandardItem*)),
	        this,
	        SLOT(onItemChanged(QStandardItem*)));
}

QAbstractItemModel * MeasurementBridge::model() const {
	return d_tcuModel;
}

void MeasurementBridge::SetExperiment(fmp::Experiment * experiment) {
	cancelAll();
	d_typeModel->clear();
	d_experiment = experiment;
	if ( d_experiment == NULL ) {
		return;
	}

	for (const auto & [MTID,type] : d_experiment->MeasurementTypes()) {
		d_typeModel->appendRow(buildType(type));
	}

	if ( d_experiment->Family() == fort::tags::Family::Undefined ) {
		return;
	}
	startAll();
}

void MeasurementBridge::onTDDAdded(const fmp::TrackingDataDirectoryConstPtr & tdd) {
	startOne(tdd);
}

void MeasurementBridge::onTDDDeleted(const QString & tddURI) {
	cancelOne(tddURI.toUtf8().data());
}

void MeasurementBridge::onFamilyChanged(fort::tags::Family f) {
	cancelAll();

	if ( f == fort::tags::Family::Undefined) {
		return;
	};
	startAll();
}

void MeasurementBridge::onThresholdChanged(uint8_t threshold) {
	cancelAll();
	startAll();
}

void MeasurementBridge::onNewTagCloseUp(fs::path tddURI,
                                        fort::tags::Family f,
                                        uint8_t threshold,
                                        fmp::TagCloseUp::ConstPtr tcu) {
	if ( d_experiment == NULL ) {
		return;
	}

	if (d_loaders.count(tddURI)  == 0
	    || f != d_experiment->Family()
	    || threshold != d_experiment->Threshold()) {
		qWarning() << "Ignoring TagCloseUp '" << tcu->URI().c_str() << "'";
		return;
	}

	addOneTCU(tddURI,tcu);
}


void MeasurementBridge::startAll() {
	if ( d_experiment == NULL ) {
		return;
	}
	for(const auto & [uri,tdd] : d_experiment->TrackingDataDirectories() ) {
		startOne(tdd);
	}

}

void MeasurementBridge::startOne(const fmp::TrackingDataDirectoryConstPtr & tdd) {
	if ( d_experiment == NULL) {
		return;
	}

	if ( d_loaders.count(tdd->URI()) != 0 ) {
		qWarning() << "Already loading '" << tdd->URI().c_str() << "'";
		return;
	}

	auto loader = new TagCloseUpLoader(tdd,
	                                   d_experiment->Family(),
	                                   d_experiment->Threshold(),
	                                   this);
	connect(loader,
	        SIGNAL(newTagCloseUp(fs::path,fort::tags::Family,uint8_t,fmp::TagCloseUp::ConstPtr)),
	        this,
	        SLOT(onNewTagCLoseUp(fs::path,fort::tags::Family,uint8_t,fmp::TagCloseUp::ConstPtr)));

	d_loaders.insert(std::make_pair(tdd->URI(),loader));
	loader->start();

}


void MeasurementBridge::cancelAll() {
	for(auto & [uri,l] : d_loaders) {
		l->cancel();
	}

	clearAllTCUs();

	for(auto & [uri,l] : d_loaders) {
		l->waitForFinished();
	}
	d_loaders.clear();
}


void MeasurementBridge::cancelOne(const fs::path & tddURI) {
	auto fi = d_loaders.find(tddURI) ;
	if ( fi == d_loaders.end() ) {
		return;
	}
	fi->second->cancel();

	clearTddTCUs(tddURI);

	fi->second->waitForFinished();
	d_loaders.erase(fi);
}

QList<QStandardItem*> MeasurementBridge::buildTag(fmp::TagID TID) const {
	auto tagItem = new QStandardItem(QString("tags/%1").arg(TID));
	tagItem->setEditable(false);
	tagItem->setData(TID,Qt::UserRole+1);
	auto dummyItem = new QStandardItem("");
	dummyItem->setEditable(false);
	dummyItem->setData(TID,Qt::UserRole+1);
	return {tagItem,dummyItem,dummyItem};
}

QList<QStandardItem*> MeasurementBridge::buildTCU(const fmp::TagCloseUp::ConstPtr & tcu) {
	if ( d_experiment == NULL ) {
		return {};
	}

	auto tcuItem = new QStandardItem(tcu->URI().c_str());
	tcuItem->setEditable(false);
	tcuItem->setData(QVariant::fromValue(tcu),Qt::UserRole+1);

	size_t mCount = countMeasurementsForTCU(tcu->URI());

	auto measurementCounts = new QStandardItem(QString("%1").arg(mCount));
	measurementCounts->setEditable(false);
	measurementCounts->setData(QVariant::fromValue(tcu),Qt::UserRole+1);
	d_counts.insert(std::make_pair(tcu->URI(),measurementCounts));
	return {tcuItem,measurementCounts};
}


void MeasurementBridge::addOneTCU(const fs::path & tddURI, const fmp::TagCloseUp::ConstPtr & tcu) {
	auto target = tcu->TagValue();

	QString tagPath = QString("tags/%1");
	tagPath = tagPath.arg(target);

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

void MeasurementBridge::clearTddTCUs(const fs::path & tddURI) {
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
	d_closeups.clear();
	d_counts.clear();
}


void MeasurementBridge::setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
                                       fmp::MeasurementType::ID MTID,
                                       QPointF start,
                                       QPointF end) {
	if ( d_experiment == NULL ) {
		return;
	}

	auto tddURI = tcu->Frame().ParentURI();
	auto fi = d_closeups.find(tddURI);
	auto ci = d_counts.find(tcu->URI());
	if ( ci == d_counts.end()
	     || fi == d_closeups.end()
	     || fi->second.count(tcu->URI()) == 0 ) {
		qWarning() << "Not setting measurement: unknwon '" << tcu->URI().c_str() << "'";
		return;
	}

	Eigen::Vector2d startFromTag = tcu->ImageToTag() * Eigen::Vector2d(start.x(),start.y());
	Eigen::Vector2d endFromTag = tcu->ImageToTag() * Eigen::Vector2d(start.x(),start.y());

	auto m = std::make_shared<fmp::Measurement>(tcu->URI(),
	                                            MTID,
	                                            startFromTag,
	                                            endFromTag,
	                                            tcu->TagSizePx());

	try {
		d_experiment->SetMeasurement(m);
	} catch (const std::exception & e ) {
		qWarning() << "Could not set measurement: " << e.what();
		return;
	}

	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcu->URI())));

	emit measurementModified(m);
}

void MeasurementBridge::deleteMeasurement(const fs::path & mURI) {
	if ( d_experiment == NULL ) {
		return;
	}
	auto tcuPath = mURI.parent_path().parent_path();
	auto ci = d_counts.find(tcuPath);
	if ( ci == d_counts.end() ) {
		return;
	}

	try {
		d_experiment->DeleteMeasurement(mURI);
	} catch (const std::exception & e) {
		qWarning() << "Could not delete measurement '" << mURI.c_str()
		           << "':" << e.what();
		return;
	}
	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcuPath)));
	emit measurementDeleted(mURI);
}


size_t MeasurementBridge::countMeasurementsForTCU(const fs::path & tcuPath) const {
	if (d_experiment == NULL){
		return 0;
	}
	auto mi = d_experiment->Measurements().find(tcuPath);
	if ( mi == d_experiment->Measurements().end() ) {
		return 0;
	}
	return mi->second.size();
}


void MeasurementBridge::setMeasurementType(int MTID, const QString & name) {
	if ( d_experiment == NULL ) {
		return;
	}
	try {
		auto fi = d_experiment->MeasurementTypes().find(MTID);
		if ( fi == d_experiment->MeasurementTypes().end() ) {
			MTID = d_experiment->NextAvailableMeasurementTypeID();
			auto type = d_experiment->CreateMeasurementType(MTID,name.toUtf8().data());
			d_typeModel->appendRow(buildType(type));
		} else {
			auto items = d_typeModel->findItems(QString::number(MTID),Qt::MatchExactly,0);
			if ( items.size() != 1 ) {
				throw std::logic_error("Internal type model error");
			}
			fi->second->SetName(name.toUtf8().data());
			d_typeModel->item(items[0]->row(),1)->setText(name);
		}
	} catch ( const std::exception & e) {
		qWarning() << "Could not set MeasurementType " << MTID << " to '" << name << "': " << e.what();
	}

	emit measurementTypeModified(MTID,name);
}

void MeasurementBridge::deleteMeasurementType(int MTID) {
	if ( d_experiment == NULL ) {
		return;
	}

	try {
		auto items = d_typeModel->findItems(QString::number(MTID),Qt::MatchExactly,0);
		if ( items.size() != 1 ) {
			throw std::logic_error("Internal type model error");
		}
		d_experiment->DeleteMeasurementType(MTID);
		d_typeModel->removeRows(items[0]->row(),1);
	} catch (const std::exception & e) {
		qWarning() << "Could not delete MeasurementType " << MTID << ": " << e.what();
	}

	emit measurementTypeDeleted(MTID);
}

QList<QStandardItem *> MeasurementBridge::buildType(const fmp::MeasurementType::Ptr & type) const {
	auto mtid =new QStandardItem(QString::number(type->MTID()));
	mtid->setEditable(false);
	mtid->setData(QVariant::fromValue(type));
	auto name = new QStandardItem(type->Name().c_str());
	mtid->setData(QVariant::fromValue(type));
	mtid->setEditable(true);
}

void MeasurementBridge::onTypeItemChanged(QStandardItem * item) {
	if (item->column() != 1) {
		return;
	}

	auto type = item->data().value<fmp::MeasurementType::Ptr>();
	std::string newName = item->text().toUtf8().data();
	if ( newName == type->Name() ) {
		return;
	}

	try {
		type->SetName(newName);
	} catch( const std::exception & e) {
		qWarning() << "Could not change measurement type " << type->MTID()
		           << ":'" << type->Name().c_str()
		           <<"' to '" << item->text()
		           << "': " << e.what();
		item->setText(type->Name().c_str());
		return;
	}

	emit measurementTypeModified(type->MTID(),type->Name().c_str());
}
