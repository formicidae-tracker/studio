#include "StatisticsBridge.hpp"

#include <QStandardItemModel>
#include <QtConcurrent>

#include <fort/studio/Format.hpp>
#include <fort/myrmidon/priv/Query.hpp>

StatisticsBridge::StatisticsBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) )
	, d_seed(0)
	, d_outdated(false)
	, d_watcher(nullptr) {
	rebuildModel();
}

StatisticsBridge::~StatisticsBridge() {
}

void StatisticsBridge::setExperiment(const fmp::Experiment::ConstPtr  & experiment) {
	d_experiment = experiment;
	emit activated(!d_experiment == false);
	setOutdated(true);
}

bool StatisticsBridge::isActive() const {
	return !d_experiment == false;
}

bool StatisticsBridge::isOutdated() const {
	return d_outdated;
}

bool StatisticsBridge::isReady() const {
	return d_watcher == nullptr;
}

QAbstractItemModel * StatisticsBridge::stats() const {
	return d_model;
}

const fm::TagStatistics & StatisticsBridge::statsForTag(fmp::TagID tagID) const {
	auto static empty =  fmp::TagStatisticsHelper::Create(0,fm::Time());
	return empty;
}


void StatisticsBridge::onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::ConstPtr tdd) {
	setOutdated(true);
}

void StatisticsBridge::onTrackingDataDirectoryDeleted(QString tddURI) {
	setOutdated(true);
}

void StatisticsBridge::rebuildModel() {
	d_model->clear();
	auto nSpaces = 0;
	if ( d_experiment ) {
		nSpaces = d_experiment->CSpaces().size();
	}
	if ( nSpaces < 2 ) {
		d_model->setHorizontalHeaderLabels({tr("Tag ID"),tr("First Seen"),tr("Last Seen"),
		                                    tr("Times Seen"),tr("Multiple Detection"),
		                                    tr("Gap <500ms"),
		                                    tr("Gap <1s"),tr("Gap <10s"),
		                                    tr("Gap <1m"),tr("Gap <10m"),
		                                    tr("Gap <1h"), tr("Gap <10h"),
		                                    tr("Gap >=10h")});
	} else {
		d_model->setHorizontalHeaderLabels({tr("Tag ID"),tr("First Seen"),tr("Last Seen"),
		                                    tr("Times Seen"),tr("Multiple Detection"),
		                                    tr("Gap <500ms"),
		                                    tr("Gap <1s"),tr("Other gaps (largely over estimated)")});
	}

	if ( !d_experiment == true ) {
		return;
	}

	for ( const auto & [tagID,tagStats] : d_stats ) {
		QList<QStandardItem*> row;
		row.push_back(new QStandardItem(QString::number(tagStats.ID)));
		row.back()->setData(tagStats.ID);
		row.push_back(new QStandardItem(ToQString(tagStats.FirstSeen)));
		row.back()->setData(ToQString(tagStats.FirstSeen));
		row.push_back(new QStandardItem(ToQString(tagStats.LastSeen)));
		row.back()->setData(ToQString(tagStats.LastSeen));

		if ( nSpaces < 2 ) {
			for ( int i = 0; i < 10; ++i) {
				row.push_back(new QStandardItem(QString::number(tagStats.Counts(i))));
				row.back()->setData(quint64(tagStats.Counts(i)));
			}
		} else {
			for ( int i = 0; i < 4; ++i) {
				row.push_back(new QStandardItem(QString::number(tagStats.Counts(i))));
				row.back()->setData(quint64(tagStats.Counts(i)));
			}
			quint64 count = tagStats.Counts.block<1,6>(4,0).sum();
			row.push_back(new QStandardItem(QString::number(count)));
			row.back()->setData(count);
		}

		for ( const auto & i : row ) {
			i->setEditable(false);
		}
		d_model->appendRow(row);
	}
}


void StatisticsBridge::setOutdated(bool outdated_) {
	if ( outdated_ == true ) {
		++d_seed;
		d_stats.clear();
		rebuildModel();
	}
	if ( d_outdated == outdated_ ) {
		return;
	}
	d_outdated = outdated_;
	emit outdated(d_outdated);
}


void StatisticsBridge::compute() {
	if ( !d_experiment == true
	     || isReady() == false
	     || d_outdated == false ) {
		return;
	}

	size_t currentSeed = d_seed;
	auto future = QtConcurrent::run([this]() -> Stats * {
		                                auto stats = new Stats();
		                                try {
			                                fmp::Query::ComputeTagStatistics(d_experiment,
			                                                                 *stats);
		                                } catch (const std::exception & e) {
			                                std::cerr << "Could not compute stats: " << e.what() <<  std::endl;
			                                return nullptr;
		                                }
		                                return stats;
	                                });

	d_watcher = new QFutureWatcher<Stats*>(this);
	connect(d_watcher,
	        &QFutureWatcher<Stats*>::finished,
	        this,
	        [this,currentSeed]() {
		        auto result = d_watcher->result();
		        d_watcher->deleteLater();
		        d_watcher = nullptr;
		        emit ready(true);

		        if ( currentSeed != d_seed
		             || result == nullptr) {
			        return;
		        }
		        d_stats = *result;
		        rebuildModel();
		        setOutdated(false);
	        },Qt::QueuedConnection);

	d_watcher->setFuture(future);
	emit ready(false);

}
