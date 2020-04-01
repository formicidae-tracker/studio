#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;

class MovieBridge : public Bridge {
	Q_OBJECT
public :
	explicit MovieBridge(QObject * parent);
	virtual ~MovieBridge();

	void setExperiment(const fmp::ExperimentConstPtr & experiment);

	bool isActive() const override;

	const QAbstractItemModel * movieModel();

	fmp::MovieSegmentConstPtr movieSegment(const QModelIndex & index) const;

public slots:
	void onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::ConstPtr & tdd);
	void onTrackingDataDirectoryDeleted(const QString & URI);

private :
	const static int PtrRole;
	const static int IDRole;


	static QList<QStandardItem*> buildSpace(const fmp::SpaceConstPtr & space);
	static QList<QStandardItem*> buildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd);
	static QList<QStandardItem*> buildMovieSegment(const fmp::MovieSegmentConstPtr & ms,
	                                               const fm::Time & start,
	                                               const fm::Time & end);

	void rebuildModel();


	fmp::ExperimentConstPtr d_experiment;
	QStandardItemModel    * d_model;
};
