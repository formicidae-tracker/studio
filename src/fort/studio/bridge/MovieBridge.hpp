#pragma once

#include <tuple>

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;

class MovieBridge : public GlobalBridge {
	Q_OBJECT
public :
	explicit MovieBridge(QObject * parent);
	virtual ~MovieBridge();

	QAbstractItemModel * movieModel();

	std::tuple<quint32,fmp::TrackingDataDirectory::Ptr,fmp::MovieSegmentConstPtr,fort::Time>
	tddAndMovieSegment(const QModelIndex & index) const;

	std::tuple<fmp::TrackingDataDirectory::Ptr,fmp::MovieSegmentConstPtr,fort::Time>
	findTime(fmp::SpaceID spaceID, const fort::Time & time);

	void initialize(ExperimentBridge * experiment) override;

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:
	void onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd);
	void onTrackingDataDirectoryDeleted(const QString & URI);

private :
	const static int PtrRole;
	const static int IDRole;
	const static int StartRole;
	const static int TddRole;
	const static int SpaceIDRole;


	static QList<QStandardItem*> buildSpace(const fmp::SpaceConstPtr & space);
	static QList<QStandardItem*> buildMovieSegment(quint32 spaceID,
	                                               const fmp::TrackingDataDirectoryPtr & tdd,
	                                               const fmp::MovieSegmentConstPtr & ms,
	                                               const fort::Time & start);

	void clearModel();
	void rebuildModel();


	QStandardItemModel    * d_model;
};
