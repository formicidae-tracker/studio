#pragma once

#include <QObject>
#include <QHash>

#include <vector>
#include <atomic>

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

	const fmp::IdentifiedFrame::ConstPtr & FrameAt(fmp::MovieFrameID movieID) const;

	void moveToThread(QThread * thread);

public slots:
	void loadMovieSegment(const fmp::TrackingDataDirectoryConstPtr & tdd,
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


	typedef QHash<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> FramesByMovieID;
	typedef std::pair<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> ConcurrentResult;

	fmp::ExperimentConstPtr d_experiment;
	FramesByMovieID         d_frames;
	int                     d_done,d_toDo;

	std::shared_ptr<std::atomic<bool>> d_abordFlag;
	size_t                             d_currentLoadingID;
	Qt::ConnectionType                 d_connectionType;
};
