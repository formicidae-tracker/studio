#include "MovieBridge.hpp"

#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/Format.hpp>

const int MovieBridge::PtrRole = Qt::UserRole+1;
const int MovieBridge::IDRole  = Qt::UserRole+2;
const int MovieBridge::StartRole  = Qt::UserRole+3;
const int MovieBridge::TddRole  = Qt::UserRole+4;

MovieBridge::MovieBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) ) {
	qRegisterMetaType<fmp::MovieSegment::ConstPtr>();
	qRegisterMetaType<fm::Time>();
	qRegisterMetaType<fmp::TrackingDataDirectory::ConstPtr>();
}


MovieBridge::~MovieBridge() {
}


void MovieBridge::setExperiment(const fmp::ExperimentConstPtr & experiment) {
	d_experiment = experiment;

	rebuildModel();
	emit activated(isActive());
}


bool MovieBridge::isActive() const {
	return !d_experiment == false;
}


QAbstractItemModel * MovieBridge::movieModel() {
	return d_model;
}


std::tuple<fmp::TrackingDataDirectory::ConstPtr,fmp::MovieSegment::ConstPtr,fm::Time>
MovieBridge::tddAndMovieSegment(const QModelIndex & index) const {
	auto item = d_model->itemFromIndex(index);
	if ( item == nullptr ) {
		return std::make_tuple(fmp::TrackingDataDirectory::ConstPtr(),fmp::MovieSegment::ConstPtr(),fm::Time());
	}
	return std::make_tuple(item->data(TddRole).value<fmp::TrackingDataDirectory::ConstPtr>(),
	                       item->data(PtrRole).value<fmp::MovieSegment::ConstPtr>(),
	                       item->data(StartRole).value<fm::Time>());
}


void MovieBridge::onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::ConstPtr & tdd) {
	rebuildModel();
}


void MovieBridge::onTrackingDataDirectoryDeleted(const QString & URI) {
	rebuildModel();
}


void MovieBridge::rebuildModel() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("URI"),tr("Start"),tr("End")});

	if ( !d_experiment ) {
		return;
	}

	for ( const auto & [spaceID,space] : d_experiment->Spaces() ) {
		d_model->appendRow(buildSpace(space));
	}
}


QList<QStandardItem*> MovieBridge::buildSpace(const fmp::SpaceConstPtr & space) {
	auto nameItem = new QStandardItem(ToQString(space->Name()));
	nameItem->setData(ToQString(space->URI()),IDRole);
	nameItem->setEditable(false);

	const auto & tdds = space->TrackingDataDirectories();

	if ( tdds.empty() == true ) {
		return {nameItem,nullptr,nullptr};
	}

	auto startItem = new QStandardItem(ToQString(tdds.front()->StartDate()));
	startItem->setEditable(false);
	auto endItem = new QStandardItem(ToQString(tdds.back()->EndDate()));
	endItem->setEditable(false);

	for ( const auto & tdd : space->TrackingDataDirectories() ){
		nameItem->appendRow(buildTDD(tdd));
	}
	return {nameItem,startItem,endItem};
}


QList<QStandardItem*> MovieBridge::buildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd) {
	auto nameItem = new QStandardItem(ToQString(tdd->URI()));
	nameItem->setEditable(false);
	nameItem->setData(ToQString(tdd->URI()),IDRole);

	auto startItem = new QStandardItem(ToQString(tdd->StartDate()));
	startItem->setEditable(false);
	auto endItem = new QStandardItem(ToQString(tdd->EndDate()));
	endItem->setEditable(false);

	for ( const auto & [ref,segment] : tdd->MovieSegments().Segments() ) {
		auto endTime = tdd->FrameReferenceAt(segment->EndFrame()).Time();
		nameItem->appendRow(buildMovieSegment(tdd,segment,ref.Time(),endTime));

	}
	return {nameItem,startItem,endItem};
}


QList<QStandardItem*> MovieBridge::buildMovieSegment(const fmp::TrackingDataDirectory::ConstPtr & tdd,
                                                     const fmp::MovieSegmentConstPtr & ms,
                                                     const fm::Time & start,
                                                     const fm::Time & end) {
	auto ptrData = QVariant::fromValue(ms);
	auto startData = QVariant::fromValue(start);
	auto tddData = QVariant::fromValue(tdd);
	auto nameItem = new QStandardItem(QString("movies/%1").arg(ms->ID()));
	nameItem->setData(ToQString(ms->URI()),IDRole);

	auto startItem = new QStandardItem(ToQString(start));

	auto endItem = new QStandardItem(ToQString(end));

	QList<QStandardItem*> res  = {nameItem,startItem,endItem};

	for ( auto & i : res ) {
		i->setEditable(false);
		i->setData(ptrData,PtrRole);
		i->setData(startData,StartRole);
		i->setData(tddData,TddRole);
	}

	return res;
}
