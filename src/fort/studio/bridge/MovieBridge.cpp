#include "MovieBridge.hpp"

#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/Format.hpp>

const int MovieBridge::PtrRole = Qt::UserRole+1;
const int MovieBridge::IDRole  = Qt::UserRole+2;

MovieBridge::MovieBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) ) {
	qRegisterMetaType<fmp::MovieSegment::ConstPtr>();
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

const QAbstractItemModel * MovieBridge::movieModel() {
	return d_model;
}

fmp::MovieSegmentConstPtr MovieBridge::movieSegment(const QModelIndex & index) const {
	auto item = d_model->itemFromIndex(index);
	if ( item == nullptr ) {
		return fmp::MovieSegment::ConstPtr();
	}
	return item->data(PtrRole).value<fmp::MovieSegment::ConstPtr>();
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
		return {nameItem};
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
		nameItem->appendRow(buildMovieSegment(segment,ref.Time(),endTime));

	}
	return {nameItem,startItem,endItem};
}
QList<QStandardItem*> MovieBridge::buildMovieSegment(const fmp::MovieSegmentConstPtr & ms,
                                                     const fm::Time & start,
                                                     const fm::Time & end) {
	auto ptrData = QVariant::fromValue(ms);
	auto nameItem = new QStandardItem(QString("movies/%1").arg(ms->ID()));
	nameItem->setEditable(false);
	nameItem->setData(ptrData,PtrRole);
	nameItem->setData(ToQString(ms->URI()),IDRole);

	auto startItem = new QStandardItem(ToQString(start));
	startItem->setEditable(false);
	startItem->setData(ptrData,PtrRole);

	auto endItem = new QStandardItem(ToQString(end));
	endItem->setEditable(false);
	endItem->setData(ptrData,PtrRole);

	return {nameItem,startItem,endItem};
}
