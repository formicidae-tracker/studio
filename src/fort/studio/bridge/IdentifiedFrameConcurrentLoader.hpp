#pragma once

#include <QObject>
#include <QHash>

#include <vector>
#include <atomic>
#include <tuple>

#include <fort/studio/MyrmidonTypes.hpp>

namespace fmp = fort::myrmidon::priv;

class IdentifiedFrameConcurrentLoader : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool done
	           READ isDone
	           NOTIFY done)
public:
	explicit IdentifiedFrameConcurrentLoader(QObject * parent = nullptr);
	~IdentifiedFrameConcurrentLoader();

	bool isDone() const;

	void setExperiment(const fmp::Experiment::ConstPtr & experiment);

	const fmp::IdentifiedFrame::ConstPtr & frameAt(fmp::MovieFrameID movieID) const;
	const fmp::InteractionFrame::ConstPtr & interactionAt(fmp::MovieFrameID movieID) const;

	void moveToThread(QThread * thread);

public slots:
	void loadMovieSegment(quint32 spaceID,
	                      const fmp::TrackingDataDirectoryConstPtr & tdd,
	                      const fmp::MovieSegmentConstPtr & segment);
	void clear();

signals:
	void progressChanged(int done,int toDo);
	void done(bool);

private slots:
	void setExperimentUnsafe(fmp::Experiment::ConstPtr experiment);
	void addDone(int done);

private :
	void abordCurrent();

	void setProgress(int done,int toDo);


	typedef QHash<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr>  FramesByMovieID;
	typedef QHash<fmp::MovieFrameID,fmp::InteractionFrame::ConstPtr> InteractionsByMovieID;
	typedef std::tuple<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr,fmp::InteractionFrame::ConstPtr> ConcurrentResult;

	fmp::ExperimentConstPtr d_experiment;
	FramesByMovieID         d_frames;
	InteractionsByMovieID   d_interactions;
	int                     d_done,d_toDo;

	std::shared_ptr<std::atomic<bool>> d_abordFlag;
	size_t                             d_currentLoadingID;
	Qt::ConnectionType                 d_connectionType;
};
