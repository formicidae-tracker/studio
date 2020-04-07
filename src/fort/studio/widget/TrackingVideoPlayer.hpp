#pragma once

#include <QObject>

#include <fort/studio/MyrmidonTypes.hpp>

#include <opencv2/videoio.hpp>

#include <vector>
#include <set>
#include <condition_variable>

#include "TrackingVideoFrame.hpp"

class TrackingVideoWidget;
class QThread;

class TrackingVideoPlayerTask;
class TrackingVideoPlayer : public QObject {
	Q_OBJECT
	Q_PROPERTY(qreal playbackRate
	           READ playbackRate
	           WRITE setPlaybackRate
	           NOTIFY playbackRateChanged)
	Q_PROPERTY(State playbackState
	           READ playbackState
	           NOTIFY playbackStateChanged)
	Q_PROPERTY(fm::Duration position
	           READ position
	           NOTIFY positionChanged);
public:
	enum class State {
		Stopped = 0,
		Playing = 1,
		Paused  = 2,
	};

	explicit TrackingVideoPlayer(QObject * parent = nullptr);
	virtual ~TrackingVideoPlayer();

	qreal playbackRate() const;

	State playbackState() const;

	fm::Duration position() const;

	fm::Duration duration() const;

	fm::Time start() const;
public slots:
	void pause();
	void play();
	void stop();
	void setMovieSegment(const fmp::MovieSegment::ConstPtr & segment,
	                     const fm::Time & start);

	void setPlaybackRate(qreal rate);

	void setPosition(fm::Duration position);

signals:
	void durationChanged(fm::Time start,fm::Duration duration);
	void positionChanged(fm::Duration duration);

	void playbackRateChanged(qreal rate);

	void playbackStateChanged(State state);

	void displayVideoFrame(TrackingVideoFrame frame);

private slots:
	void onNewVideoFrame(size_t taskID, size_t localIndex, TrackingVideoFrame frame);

	void onTimerTimeout();
private:
	void sendToProcess(TrackingVideoFrame frame);

	TrackingVideoPlayerTask   * d_task;
	State                       d_state;
	fmp::MovieSegment::ConstPtr d_segment;
	qreal                       d_rate;
	QThread                   * d_movieThread;
	QTimer                    * d_timer;
	fm::Time                    d_start;
	fm::Duration                d_interval;
	fm::Duration                d_position;
	fm::Duration                d_duration;

	bool                        d_displayNext;

	size_t                          d_currentTaskID;
	size_t                          d_currentSeekID;
	std::vector<TrackingVideoFrame> d_frames,d_stagging;
	std::mutex                      d_seekMutex;
};


class TrackingVideoPlayerTask : public QObject {
Q_OBJECT
public:
	explicit TrackingVideoPlayerTask(size_t taskID, const fmp::MovieSegment::ConstPtr & segment);

	virtual ~TrackingVideoPlayerTask();

	double fps() const;
	qint64 numberOfFrame() const;

	std::shared_ptr<QImage> allocate() const;

	void processNewFrame(TrackingVideoFrame frame);

	void seek(size_t seekID, fm::Duration);

signals:
	void newFrame(size_t taskID, size_t seekID, TrackingVideoFrame frame);

private slots:
	void seekUnsafe(size_t seekID, fm::Duration);

	void processNewFrameUnsafe(TrackingVideoFrame frame);

private:
	cv::VideoCapture        d_capture;
	std::condition_variable d_condition;
	int                     d_width,d_height;
	size_t                  d_taskID,d_seekID;
};
