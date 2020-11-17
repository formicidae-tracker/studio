#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>

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

	size_t frameCount() const;


public slots:


	void onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::Ptr tdd);
	void onTrackingDataDirectoryDeleted(QString tddURI);

private:
	typedef fm::TagStatistics::ByTagID Stats;

	void compute();


	void rebuildModel();
	void recountFrames();
	fmp::Experiment::ConstPtr  d_experiment;
	QStandardItemModel       * d_model;
	bool                       d_outdated;
	QFutureWatcher<Stats*>   * d_watcher;

	Stats                      d_stats;
	size_t                     d_frameCount;
};
