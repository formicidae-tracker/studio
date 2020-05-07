#include "StatisticsBridge.hpp"

#include <QStandardItemModel>
#include <QtConcurrent>

#include <fort/studio/Format.hpp>

StatisticsBridge::StatisticsBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) ) {
	rebuildModel();
}

StatisticsBridge::~StatisticsBridge() {
	for ( auto & [tddURI,watcher] : d_watchers ) {
		watcher->cancel();
		watcher->waitForFinished();
	}
}

void StatisticsBridge::setExperiment(const fmp::Experiment::ConstPtr  & experiment) {
	d_experiment = experiment;
	emit activated(!d_experiment == false);

	for ( const auto & [tddURI,tdd] : d_experiment->TrackingDataDirectories() ) {
		onTrackingDataDirectoryAdded(tdd);
	}
}

bool StatisticsBridge::isActive() const {
	return !d_experiment == false;
}

QAbstractItemModel * StatisticsBridge::stats() const {
	return d_model;
}

fmp::TagStatistics::ConstPtr StatisticsBridge::statsForTag(fmp::TagID tagID) const {
	return fmp::TagStatistics::ConstPtr();
}


void StatisticsBridge::onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::ConstPtr tdd) {
	QString uri = tdd->URI().c_str();
	if ( d_watchers.count(uri) != 0 ) {
		return;
	}
	d_watchers.insert(std::make_pair(uri,new QFutureWatcher<TimedStats>(this)));
	d_stats.insert(std::make_pair(uri,Stats()));
	auto watcher = d_watchers.at(uri);
	connect(watcher,&QFutureWatcher<TimedStats>::finished,
	        this,
	        [this,watcher,uri] () {
		        if ( watcher->isCanceled() == true || d_stats.count(uri) == 0 ) {
			        return;
		        }
		        std::vector<TimedStats> stats;
		        stats.reserve(watcher->progressMaximum());
		        for ( int i = 0; i < watcher->progressMaximum(); ++i ) {
			        stats.push_back(watcher->resultAt(i));
		        }
		        d_stats.at(uri) = fmp::TagStatistics::MergeTimed(stats.begin(),stats.end());
		        rebuildModel();
	        },
	        Qt::QueuedConnection);

	const auto & segments = tdd->TrackingSegments().Segments();
	d_files[uri].reserve(segments.size());
	for ( const auto & [ref,segment] : segments ) {
		QString filepath = (tdd->AbsoluteFilePath() / segment).c_str();
		d_files[uri].push_back(filepath);
	}
	watcher->setFuture(QtConcurrent::mapped(d_files[uri],&Load));
	return;
}

void StatisticsBridge::onTrackingDataDirectoryDeleted(QString tddURI) {
	auto fi = d_watchers.find(tddURI);
	if ( fi == d_watchers.end() ) {
		return;
	}
	const auto & watcher = fi->second;
	watcher->cancel();
	watcher->waitForFinished();

	delete watcher;

	d_watchers.erase(tddURI);
	d_stats.erase(tddURI);
	d_files.erase(tddURI);
	rebuildModel();
}


StatisticsBridge::TimedStats StatisticsBridge::Load(QString filepath) {
	try {
		auto res = fmp::TagStatistics::BuildStats(ToStdString(filepath));
		return res;
	} catch ( const std::exception & e) {
		qWarning() << "Could not build statistics for "
		           << filepath
		           << ": " << e.what();
	}
	return TimedStats();
}

void StatisticsBridge::rebuildModel() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("Tag ID"),tr("First Seen"),tr("Last Seen"),
	                                    tr("Times Seen"),tr("Multiple Detection"),
	                                    tr("Gap <500ms"),
	                                    tr("Gap <1s"),tr("Gap <10s"),
	                                    tr("Gap <1m"),tr("Gap <10m"),
	                                    tr("Gap <1h"), tr("Gap <10h"),
	                                    tr("Gap >=10h")});
	if ( !d_experiment == true ) {
		return;
	}

	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);

	std::vector<Stats> spaceStats;
	for ( const auto & [spaceID,space] : d_experiment->CSpaces() ) {
		spaceStats.reserve(spaceStats.size() + space->TrackingDataDirectories().size());
		for ( const auto & tdd : space->TrackingDataDirectories() ) {
			QString uri = tdd->URI().c_str();
			try {
				spaceStats.push_back(d_stats.at(uri));
			} catch (const std::exception & e) {
				qWarning() << "Missing stats for " << uri;
			}
		}

	}

	auto stats = fmp::TagStatistics::MergeSpaced(spaceStats.begin(),spaceStats.end());

	for ( const auto & [tagID,tagStats] : stats ) {
		QList<QStandardItem*> row;
		row.push_back(new QStandardItem(QString::number(tagStats->ID)));
		row.back()->setData(tagStats->ID);
		row.push_back(new QStandardItem(ToQString(tagStats->FirstSeen)));
		row.back()->setData(ToQString(tagStats->FirstSeen));
		row.push_back(new QStandardItem(ToQString(tagStats->LastSeen)));
		row.back()->setData(ToQString(tagStats->LastSeen));
		for ( int i = 0; i < 10; ++i) {
			row.push_back(new QStandardItem(QString::number(tagStats->Counts(i))));
			row.back()->setData(quint64(tagStats->Counts(i)));

		}
		for ( const auto & i : row ) {
			i->setEditable(false);
		}
		d_model->appendRow(row);
	}
}
