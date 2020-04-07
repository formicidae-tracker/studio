#pragma once

#include <QObject>
#include <QHash>

#include <vector>
#include <atomic>

#include <fort/myrmidon/priv/ForwardDeclaration.hpp>
#include <fort/myrmidon/priv/Types.hpp>

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

	void setExperiment(const fmp::ExperimentConstPtr & experiment);

	const fmp::IdentifiedFrame::ConstPtr & FrameAt(fmp::MovieFrameID movieID) const;

public slots:
	void loadMovieSegment(const fmp::TrackingDataDirectoryConstPtr & tdd,
	                      const fmp::MovieSegmentConstPtr & segment);
	void clear();

signals:
	void progressChanged(int done,int toDo);
	void done(bool);

private :
	void abordCurrent();

	void setProgress(int done,int toDo);

	typedef QHash<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> FramesByMovieID;
	typedef std::pair<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> ConcurrentResult;

	fmp::ExperimentConstPtr d_experiment;
	FramesByMovieID         d_frames;
	int                     d_done,d_toDo;

	std::vector<std::shared_ptr<std::atomic<bool>>> d_abordFlags;
	size_t                                          d_currentLoading;
};
