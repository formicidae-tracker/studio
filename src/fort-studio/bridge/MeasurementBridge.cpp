#include "MeasurementBridge.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort-studio/Format.hpp>

#include <QtConcurrent>
#include <QDebug>


TagCloseUpLoader::TagCloseUpLoader(const fmp::TrackingDataDirectoryConstPtr & tdd,
                                   fort::tags::Family f,
                                   uint8_t threshold,
                                   QObject * parent)
	: QObject(parent)
	, d_tddURI(tdd->URI())
	, d_futureWatcher(new QFutureWatcher<fmp::TagCloseUp::List>(this))
	, d_lister(tdd->TagCloseUpLister(f,threshold))
	, d_toDo(0)
	, d_done(0) {
	connect(d_futureWatcher,
	        &QFutureWatcher<fmp::TagCloseUp::List>::resultReadyAt,
	        this,
	        &TagCloseUpLoader::onResultReady);
}

size_t TagCloseUpLoader::toDo() const {
	return d_toDo;
}

size_t TagCloseUpLoader::done() const {
	return d_done;
}

void TagCloseUpLoader::waitForFinished() {
	d_futureWatcher->waitForFinished();
}

fmp::TagCloseUp::List TagCloseUpLoader::load(const fmp::TagCloseUp::Lister::Loader & l) {
	return l();
}

void TagCloseUpLoader::cancel() {
	d_futureWatcher->cancel();
}

void TagCloseUpLoader::start() {
	qInfo() << "Starting tag close-up loaders for TDD:'" << d_tddURI.c_str() << "'";
	auto loaders = d_lister->PrepareLoaders();
	d_done = 0;
	d_toDo = loaders.size();
	d_futureWatcher->setFuture(QtConcurrent::mapped(loaders,TagCloseUpLoader::load));
}

void TagCloseUpLoader::onResultReady(int index) {
	auto tags = d_futureWatcher->resultAt(index);
	d_done += 1;
	for (auto & t : tags) {
		emit newTagCloseUp(d_tddURI,d_lister->Family(),d_lister->Threshold(),t);
	}
	if ( d_done == d_toDo ) {
		qInfo() << "Finished all tag close-up loaders for TDD:'"
		        << d_tddURI.c_str() << "'";
	}
	emit progressChanged(d_done,d_done-1);
}


MeasurementBridge::MeasurementBridge(QObject * parent)
	: Bridge(parent)
	, d_tcuModel( new QStandardItemModel (this) )
	, d_typeModel( new QStandardItemModel (this) )
	, d_toDo(0)
	, d_done(0) {

	connect(d_typeModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &MeasurementBridge::onTypeItemChanged);
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
	cancelAll();
	d_typeModel->clear();
	d_typeModel->setHorizontalHeaderLabels({tr("ID"),tr("Name")});
	d_experiment = experiment;
	d_toDo = 0;
	d_done = 0;
	if ( !d_experiment ) {
		emit activated(false);
		emit progressChanged(d_done,d_toDo);
		return;
	}

	for (const auto & [MTID,type] : d_experiment->MeasurementTypes()) {
		d_typeModel->appendRow(buildType(type));
	}

	emit activated(true);

	if ( d_experiment->Family() == fort::tags::Family::Undefined ) {
		emit progressChanged(d_done,d_toDo);
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

void MeasurementBridge::onDetectionSettingChanged(fort::tags::Family , uint8_t) {
	if ( d_experiment ) {
		return;
	}
	qDebug() << "[MeasurementBridge]: newDetectionSetting '"
	         << int(d_experiment->Family()) << ";"
	         << d_experiment->Threshold();

	cancelAll();

	if ( d_experiment->Family() == fort::tags::Family::Undefined) {
		qDebug() << "[MeasurementBridge]: No tag family defined, not starting loaders";
		return;
	};

	startAll();
}


void MeasurementBridge::onNewTagCloseUp(std::string tddURI,
                                        fort::tags::Family f,
                                        uint8_t threshold,
                                        fmp::TagCloseUp::ConstPtr tcu) {
	if ( !d_experiment ) {
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
	if ( !d_experiment ) {
		return;
	}

	for(const auto & [uri,tdd] : d_experiment->TrackingDataDirectories() ) {
		startOne(tdd);
	}

}

void MeasurementBridge::startOne(const fmp::TrackingDataDirectoryConstPtr & tdd) {
	if ( !d_experiment ) {
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
	        &TagCloseUpLoader::newTagCloseUp,
	        this,
	        &MeasurementBridge::onNewTagCloseUp);

	connect(loader,
	        &TagCloseUpLoader::progressChanged,
	        this,
	        &MeasurementBridge::onLoaderProgressChanged);

	d_loaders.insert(std::make_pair(tdd->URI(),loader));
	loader->start();
	d_toDo += loader->toDo();
	emit progressChanged(d_done,d_toDo);
}


void MeasurementBridge::cancelAll() {
	qInfo() << "Cancelling all tag close-up loaders";
	for(auto & [uri,l] : d_loaders) {
		l->cancel();
	}

	clearAllTCUs();

	for(auto & [uri,l] : d_loaders) {
		l->waitForFinished();
	}

	d_loaders.clear();
	d_toDo = 0;
	d_done = 0;

	emit progressChanged(d_toDo,d_done);
}


void MeasurementBridge::cancelOne(const std::string & tddURI) {
	qInfo() << "Cancelling tag close-up loaders for TDD:'" << tddURI.c_str() << "'";
	auto fi = d_loaders.find(tddURI) ;
	if ( fi == d_loaders.end() ) {
		return;
	}
	fi->second->cancel();

	clearTddTCUs(tddURI);

	d_toDo -= fi->second->toDo();
	d_done -= fi->second->done();

	fi->second->waitForFinished();

	d_loaders.erase(fi);

	emit progressChanged(d_done,d_toDo);
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
	if ( !d_experiment ) {
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


void MeasurementBridge::addOneTCU(const std::string & tddURI,
                                  const fmp::TagCloseUp::ConstPtr & tcu) {
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
	d_closeups.clear();
	d_counts.clear();
}


void MeasurementBridge::setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
                                       fmp::MeasurementType::ID MTID,
                                       QPointF start,
                                       QPointF end) {
	if ( !d_experiment ) {
		return;
	}

	auto tddURI = tcu->Frame().ParentURI();
	auto fi = d_closeups.find(tddURI);
	auto ci = d_counts.find(tcu->URI());
	if ( ci == d_counts.end()
	     || fi == d_closeups.end()
	     || fi->second.count(tcu->URI()) == 0 ) {
		qWarning() << "Not setting measurement: unknown '" << tcu->URI().c_str() << "'";
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
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::SetMeasurement('"
		         << m->URI().c_str() << "')";
		d_experiment->SetMeasurement(m);
	} catch (const std::exception & e ) {
		qCritical() << "Could not set measurement '"
		            << m->URI().c_str() << "': " << e.what();
		return;
	}


	qInfo() << "Set measurement '" << m->URI().c_str() << "'";
	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcu->URI())));

	setModified(true);
	emit measurementModified(m);
}

void MeasurementBridge::deleteMeasurement(const std::string & mURI) {
	if ( !d_experiment ) {
		return;
	}
	auto tcuPath = fs::path(mURI).parent_path().parent_path().generic_string();
	auto ci = d_counts.find(tcuPath);
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
	ci->second->setText(QString("%1").arg(countMeasurementsForTCU(tcuPath)));

	qInfo() << "Deleted measurement '" << mURI.c_str() << "'";
	setModified(true);
	emit measurementDeleted(mURI);
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


void MeasurementBridge::setMeasurementType(quint32 MTID, const QString & name) {
	if ( !d_experiment ) {
		return;
	}
	try {
		auto fi = d_experiment->MeasurementTypes().find(MTID);
		if ( fi == d_experiment->MeasurementTypes().end() ) {
			qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::Experiment::CreateMeasurement('" << name << "')";
			auto type = d_experiment->CreateMeasurementType(ToStdString(name));
			MTID = type->MTID();
			d_typeModel->appendRow(buildType(type));
		} else {
			auto items = d_typeModel->findItems(QString::number(MTID),Qt::MatchExactly,0);
			if ( items.size() != 1 ) {
				throw std::logic_error("Internal type model error");
			}
			qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::priv::MeasurementType::SetName('" << name << "')";
			fi->second->SetName(name.toUtf8().data());
			d_typeModel->item(items[0]->row(),1)->setText(name);
		}
	} catch ( const std::exception & e) {
		qCritical() << "Could not set MeasurementType " << MTID << " to '" << name << "': " << e.what();
	}

	qInfo() << "Set MeasurementType " << MTID
	        << " name to '" << name << "'";
	setModified(true);
	emit measurementTypeModified(MTID,name);
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

void MeasurementBridge::deleteMeasurementType(quint32 MTID) {
	if ( !d_experiment ) {
		return;
	}

	try {
		auto items = d_typeModel->findItems(QString::number(MTID),Qt::MatchExactly,0);
		if ( items.size() != 1 ) {
			throw std::logic_error("Internal type model error");
		}
		qDebug() << "[MeasurementBridge]: Calling fort::myrmidon::Experiment::DeleteMeasurementType("
		         << MTID << ")";
		d_experiment->DeleteMeasurementType(MTID);
		d_typeModel->removeRows(items[0]->row(),1);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete MeasurementType " << MTID << ": " << e.what();
		return;
	}

	qInfo() << "Deleted MeasurementType " << MTID;
	setModified(true);
	emit measurementTypeDeleted(MTID);
}

QList<QStandardItem *> MeasurementBridge::buildType(const fmp::MeasurementType::Ptr & type) const {
	auto mtid =new QStandardItem(QString::number(type->MTID()));
	mtid->setEditable(false);
	mtid->setData(QVariant::fromValue(type));
	auto name = new QStandardItem(type->Name().c_str());
	name->setData(QVariant::fromValue(type));
	name->setEditable(true);
	return {mtid,name};
}

void MeasurementBridge::onTypeItemChanged(QStandardItem * item) {
	if (item->column() != 1) {
		qDebug() << "[MeasurmentBridge]: Ignoring measurement type item change for column " << item->column();
		return;
	}

	auto type = item->data().value<fmp::MeasurementType::Ptr>();
	std::string newName = item->text().toUtf8().data();
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

void MeasurementBridge::onLoaderProgressChanged(size_t done, size_t oldDone) {
	d_done += done - oldDone;
	emit progressChanged(d_done,d_toDo);
}
