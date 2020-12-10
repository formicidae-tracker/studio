#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>

#include <QFutureWatcher>

class QStandardItemModel;
class QAbstractItemModel;

class StatisticsBridge : public GlobalBridge {
	Q_OBJECT

public:
	StatisticsBridge(QObject * parent );
	virtual ~StatisticsBridge();

	void initialize(ExperimentBridge * experiment) override;

	QAbstractItemModel * stats() const;

	const fm::TagStatistics & statsForTag(fmp::TagID tagID) const;

	size_t frameCount() const;

public slots:
	void onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::Ptr tdd);
	void onTrackingDataDirectoryDeleted(QString tddURI);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private:
	typedef fm::TagStatistics::ByTagID Stats;

	void clear();
	void compute();
	void rebuildModel();
	void recountFrames();

	QStandardItemModel       * d_model;
	bool                       d_outdated;
	QFutureWatcher<Stats*>   * d_watcher;

	Stats                      d_stats;
	size_t                     d_frameCount;
};
