#pragma once

#include <tuple>

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

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

	QAbstractItemModel * movieModel();

	std::tuple<quint32,fmp::TrackingDataDirectory::Ptr,fmp::MovieSegmentConstPtr,fm::Time>
	tddAndMovieSegment(const QModelIndex & index) const;

	std::tuple<fmp::TrackingDataDirectory::Ptr,fmp::MovieSegmentConstPtr,fm::Time>
	findTime(fmp::SpaceID spaceID, const fm::Time & time);

public slots:
	void onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd);
	void onTrackingDataDirectoryDeleted(const QString & URI);

private :
	const static int PtrRole;
	const static int IDRole;
	const static int StartRole;
	const static int TddRole;
	const static int SpaceIDRole;


	static QList<QStandardItem*> buildSpace(const fmp::SpaceConstPtr & space);
	static QList<QStandardItem*> buildTDD(quint32 spaceID,
	                                      const fmp::TrackingDataDirectoryPtr & tdd);
	static QList<QStandardItem*> buildMovieSegment(quint32 spaceID,
	                                               const fmp::TrackingDataDirectoryPtr & tdd,
	                                               const fmp::MovieSegmentConstPtr & ms,
	                                               const fm::Time & start,
	                                               const fm::Time & end);

	void rebuildModel();


	fmp::ExperimentConstPtr d_experiment;
	QStandardItemModel    * d_model;
};
