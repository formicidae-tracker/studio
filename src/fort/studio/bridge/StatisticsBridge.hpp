#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>
#include <QFutureWatcher>

class QStandardItemModel;
class QAbstractItemModel;

class StatisticsBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(bool isOutdated
	           READ isOutdated
	           NOTIFY outdated)
	Q_PROPERTY(bool isReady
	           READ isReady
	           NOTIFY ready)

public:
	StatisticsBridge(QObject * parent );
	virtual ~StatisticsBridge();

	void setExperiment(const fmp::Experiment::ConstPtr  & experiment);

	bool isActive() const override;

	bool isOutdated() const;
	bool isReady() const;

	QAbstractItemModel * stats() const;

	const fm::TagStatistics & statsForTag(fmp::TagID tagID) const;

signals:
	void outdated(bool);
	void ready(bool);

public slots:
	void compute();


	void onTrackingDataDirectoryAdded(fmp::TrackingDataDirectory::ConstPtr tdd);
	void onTrackingDataDirectoryDeleted(QString tddURI);

private:
	typedef fm::TagStatistics::ByTagID Stats;
	void setOutdated(bool outdated);


	void rebuildModel();

	fmp::Experiment::ConstPtr  d_experiment;
	QStandardItemModel       * d_model;
	bool                       d_outdated;
	QFutureWatcher<Stats*>   * d_watcher;
	Stats                      d_stats;
	size_t                     d_seed;
};
