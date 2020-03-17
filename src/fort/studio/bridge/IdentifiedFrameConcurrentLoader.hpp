#pragma once

#include <QObject>
#include <QHash>
#include <QFutureWatcher>

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
	void done(bool);
private slots:

	void onFinished();
	void onResultReadyAt(int index);

private :
	friend class IdentifiedFrameComputer;
	typedef QHash<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> FramesByMovieID;
	typedef std::pair<fmp::MovieFrameID,fmp::IdentifiedFrame::ConstPtr> MappedResult;

	void setDone(bool done);

	bool                    d_done;
	fmp::ExperimentConstPtr d_experiment;
	FramesByMovieID         d_frames;

	QFutureWatcher<MappedResult> * d_futureWatcher;
};
