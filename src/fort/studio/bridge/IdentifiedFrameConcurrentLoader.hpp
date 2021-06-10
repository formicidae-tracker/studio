#pragma once

#include <QObject>
#include <QHash>

#include <vector>
#include <atomic>
#include <tuple>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/MovieSegment.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class IdentifiedFrameConcurrentLoader : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool done
	           READ isDone
	           NOTIFY done)
public:
	explicit IdentifiedFrameConcurrentLoader(QObject * parent = nullptr);
	virtual ~IdentifiedFrameConcurrentLoader();

	bool isDone() const;

	void setExperiment(const fmp::Experiment::ConstPtr & experiment);

	const fm::IdentifiedFrame::Ptr & frameAt(fmp::MovieFrameID movieID) const;
	const fm::CollisionFrame::Ptr & collisionAt(fmp::MovieFrameID movieID) const;


	void moveToThread(QThread * thread);

public slots:
	void loadMovieSegment(quint32 spaceID,
	                      const fmp::TrackingDataDirectoryPtr & tdd,
	                      const fmp::MovieSegmentConstPtr & segment);
	void clear();

	quint64 findAnt(quint32 antID,
	                quint64 frameID,
	                int direction);
signals:
	void progressChanged(int done,int toDo);
	void done(bool);

private slots:
	void setExperimentUnsafe(fmp::Experiment::ConstPtr experiment);
	void addDone(int done);

private :
	void abordCurrent();

	void setProgress(int done,int toDo);


	typedef fmp::DenseMap<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr>  FramesByMovieID;
	typedef fmp::DenseMap<fmp::MovieFrameID,fm::CollisionFrame::Ptr> CollisionsByMovieID;
	typedef std::tuple<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr,fm::CollisionFrame::Ptr> ConcurrentResult;

	fmp::ExperimentConstPtr d_experiment;
	FramesByMovieID         d_frames;
	CollisionsByMovieID     d_collisions;
	int                     d_done,d_toDo;

	std::shared_ptr<std::atomic<bool>> d_abordFlag;
	size_t                             d_currentLoadingID;
	Qt::ConnectionType                 d_connectionType;
};
