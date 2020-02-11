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
	, d_model( new QStandardItemModel (this) )
	, d_experiment(NULL) {
}

QAbstractItemModel * MeasurementBridge::model() const {
	return d_model;
}

void MeasurementBridge::SetExperiment(fmp::Experiment * experiment) {
	cancelAll();
	d_experiment = experiment;
	if ( d_experiment == NULL || d_experiment->Family() == fort::tags::Family::Undefined ) {
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

QList<QStandardItem*> MeasurementBridge::BuildTag(fmp::TagID TID) {
	auto tagItem = new QStandardItem(QString("tags/%1").arg(TID));
	tagItem->setEditable(false);
	tagItem->setData(TID,Qt::UserRole+1);
	auto dummyItem = new QStandardItem("");
	dummyItem->setEditable(false);
	dummyItem->setData(TID,Qt::UserRole+1);
	return {tagItem,dummyItem,dummyItem};
}

QList<QStandardItem*> MeasurementBridge::BuildTCU(const fmp::TagCloseUp::ConstPtr & tcu) {
	if ( d_experiment == NULL ) {
		return {};
	}

	auto tcuItem = new QStandardItem(tcu->URI().c_str());
	tcuItem->setEditable(false);
	tcuItem->setData(QVariant::fromValue(tcu),Qt::UserRole+1);

	size_t mCount = 0;
	auto fi = d_experiment->Measurements().find(tcu->URI());
	if ( fi != d_experiment->Measurements().end() ) {
		mCount = fi->second.size();
	}

	auto measurementCounts = new QStandardItem(QString("%1").arg(mCount));
	measurementCounts->setEditable(false);
	measurementCounts->setData(QVariant::fromValue(tcu),Qt::UserRole+1);
	return {tcuItem,measurementCounts};
}


void MeasurementBridge::addOneTCU(const fs::path & tddURI, const fmp::TagCloseUp::ConstPtr & tcu) {
	auto target = tcu->TagValue();

	QString tagPath = QString("tags/%1");
	tagPath = tagPath.arg(target);

	auto items = d_model->findItems(tagPath);
	QStandardItem * tagItem = NULL;
	if ( items.size() == 0 ) {
		auto tagItems = BuildTag(target);
		tagItem = tagItems[0];
		d_model->invisibleRootItem()->appendRow(tagItems);
	} else {
		tagItem = items[0];
	}

	tagItem->appendRow(BuildTCU(tcu));

	auto fi = d_closeups.insert(std::make_pair(tddURI,CloseUpByPath()));
	fi.first->second.insert(std::make_pair(tcu->URI(),tcu));
}

void MeasurementBridge::clearTddTCUs(const fs::path & tddURI) {
	auto fi = d_closeups.find(tddURI);
	if ( fi == d_closeups.end() ){
		return;
	}
	for ( const auto & [uri,tcu] : fi->second ){
		auto items = d_model->findItems(uri.c_str());
		for(const auto item : items) {
			auto index = item->index();
			d_model->removeRows(index.row(),1,index.parent());
		}
	}
	d_closeups.erase(fi);
}

void MeasurementBridge::clearAllTCUs() {
	d_model->clear();
	d_closeups.clear();
}
