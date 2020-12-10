#pragma once

#include <QObject>

#include <fort/studio/MyrmidonTypes/MovieSegment.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

#include <opencv2/videoio.hpp>

#include <deque>
#include <set>
#include <condition_variable>

#include "TrackingVideoFrame.hpp"

class IdentifiedFrameConcurrentLoader;
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
	Q_PROPERTY(bool scrollMode
	           READ scrollMode
	           WRITE setScrollMode);

public:
	enum class State {
		Stopped = 0,
		Playing = 1,
		Paused  = 2,
	};

	explicit TrackingVideoPlayer(QObject * parent = nullptr);
	virtual ~TrackingVideoPlayer();


	void setup(IdentifiedFrameConcurrentLoader * loader);

	qreal playbackRate() const;

	State playbackState() const;

	fm::Duration position() const;

	fm::Duration duration() const;

	fm::Time start() const;

	bool isSeekReady() const;

	bool scrollMode() const;

	const fmp::MovieSegment::ConstPtr & currentSegment() const;

public slots:

	void togglePlayPause();
	void pause();
	void play();
	void stop();
	void setMovieSegment(quint32 spaceID,
	                     const fmp::TrackingDataDirectory::Ptr & tdd,
	                     const fmp::MovieSegment::ConstPtr & segment,
	                     const fm::Time & start);

	void clearMovieSegment();

	void setPlaybackRate(qreal rate);

	void setPosition(fm::Duration position);

	void jumpNextFrame();
	void jumpPrevFrame();

	void skipDuration(fm::Duration duration);

	void setTime(const fm::Time & time);

	void setScrollMode(bool scrollMode);

	void jumpNextVisible(fmp::AntID antID, bool backward);


signals:
	void seekReady(bool ready);

	void durationChanged(fm::Time start,fm::Duration duration,double fps);
	void positionChanged(fm::Duration duration);

	void playbackRateChanged(qreal rate);

	void playbackStateChanged(State state);

	void displayVideoFrame(TrackingVideoFrame frame);

private slots:
	void onNewVideoFrame(size_t taskID, size_t localIndex, TrackingVideoFrame frame);

	void onTimerTimeout();

	void setSeekReady(bool value);
private:
	const static size_t BUFFER_SIZE = 3;

	void sendToProcess(TrackingVideoFrame frame);

	void stopTask();
	void bootstrapTask(quint32 spaceID,
	                   const fmp::TrackingDataDirectory::Ptr & tdd);

	void displayVideoFrameImpl(const TrackingVideoFrame & frame);

	static size_t computeRate(double rate);

	TrackingVideoPlayerTask             * d_task;
	IdentifiedFrameConcurrentLoader     * d_loader;

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
	bool                        d_scrollMode;

	size_t                          d_currentTaskID;
	size_t                          d_currentSeekID;
	std::deque<TrackingVideoFrame>  d_frames,d_stagging;
	TrackingVideoFrame              d_displayed;
	bool                            d_seekReady;
};


class TrackingVideoPlayerTask : public QObject {
Q_OBJECT
public:
	explicit TrackingVideoPlayerTask(size_t taskID,
	                                 const fmp::MovieSegment::ConstPtr & segment,
	                                 size_t rate,
	                                 IdentifiedFrameConcurrentLoader * loader);

	virtual ~TrackingVideoPlayerTask();

	double fps() const;
	qint64 numberOfFrame() const;

	std::shared_ptr<QImage> allocate() const;

	void startLoadingFrom(quint32 spaceID,
	                      const fmp::TrackingDataDirectory::Ptr & tdd);

	void processNewFrame(TrackingVideoFrame frame);

	void seek(size_t seekID, fm::Duration);

	void setRate(size_t rate);

signals:
	void newFrame(size_t taskID, size_t seekID, TrackingVideoFrame frame);

private slots:
	void seekUnsafe(size_t seekID, fm::Duration);

	void processNewFrameUnsafe(TrackingVideoFrame frame);

	void startLoadingFromUnsafe(quint32 spaceID,
	                            fmp::TrackingDataDirectory::Ptr tdd);

	void setRateUnsafe(size_t rate);

private:
	fmp::MovieSegment::ConstPtr       d_segment;
	cv::VideoCapture                  d_capture;
	IdentifiedFrameConcurrentLoader * d_loader;
	int                               d_width,d_height;
	size_t                            d_taskID,d_seekID,d_rate;
	fm::Duration                      d_expectedFrameDuration;
};
