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

	fmp::TagStatistics::ConstPtr statsForTag(fmp::TagID tagID) const;

public slots:
	void onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::ConstPtr tdd);
	void onTrackingDataDirectoryDeleted(QString tddURI);

private:
	typedef fmp::TagStatistics::ByTagID                    Stats;
	typedef fmp::TagStatistics::Timed                      TimedStats;
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
