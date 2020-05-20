#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

#include <QFutureWatcher>

class QStandardItemModel;
class QAbstractItemModel;

class StatisticsBridge : public Bridge {
	Q_OBJECT
public:
	StatisticsBridge(QObject * parent );
	virtual ~StatisticsBridge();

	void setExperiment(const fmp::Experiment::ConstPtr  & experiment);

	bool isActive() const override;

	QAbstractItemModel * stats() const;

	const fm::TagStatistics & statsForTag(fmp::TagID tagID) const;

public slots:
	void onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::ConstPtr tdd);
	void onTrackingDataDirectoryDeleted(QString tddURI);

private:
	typedef fm::TagStatistics::ByTagID                     Stats;
	typedef fmp::TagStatisticsHelper::Timed                TimedStats;
	typedef std::map<QString,Stats>                        StatsByTddURI;
	typedef std::map<QString,QFutureWatcher<TimedStats>* > Watchers;
	typedef std::map<QString,std::vector<QString>>         Files;
	static TimedStats Load(QString data);

	void rebuildModel();


	fmp::Experiment::ConstPtr d_experiment;
	QStandardItemModel      * d_model;
	Watchers                  d_watchers;
	Files                     d_files;
	StatsByTddURI             d_stats;
};
