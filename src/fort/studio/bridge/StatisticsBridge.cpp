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
	compute();
}

bool StatisticsBridge::isActive() const {
	return !d_experiment == false;
}

QAbstractItemModel * StatisticsBridge::stats() const {
	return d_model;
}

const fm::TagStatistics & StatisticsBridge::statsForTag(fmp::TagID tagID) const {
	auto fi  = d_stats.find(tagID);
	if ( fi == d_stats.cend() ) {
		static auto empty = fmp::TagStatisticsHelper::Create(0,fm::Time());
		return empty;
	}
	return fi->second;
}


void StatisticsBridge::onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::Ptr tdd) {
	compute();
}

void StatisticsBridge::onTrackingDataDirectoryDeleted(QString tddURI) {
	compute();
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
		row.push_back(new QStandardItem(fmp::FormatTagID(tagStats.ID).c_str()));
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



void StatisticsBridge::compute() {
	d_stats.clear();
	if ( !d_experiment == false ) {
		try {
			fmp::Query::ComputeTagStatistics(d_experiment,d_stats);
		} catch ( const std::exception & e ) {
			qCritical() << "Could not compute tag statistics: " << e.what();
		}
	}
	rebuildModel();
}
