#include "TrackingVideoPlayer.hpp"


#include <QDebug>
#include <QTimer>
#include <QThread>
#include <limits>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/IdentifiedFrameConcurrentLoader.hpp>

#include "TrackingVideoWidget.hpp"

TrackingVideoPlayer::TrackingVideoPlayer(QObject * parent)
	: QObject(parent)
	, d_task(nullptr)
	, d_state(State::Stopped)
	, d_rate(1.0)
	, d_movieThread(new QThread())
	, d_timer(new QTimer(this))
	, d_currentTaskID(0)
	, d_seekReady(true)
	, d_scrollMode(false) {
	d_movieThread->start();
	qRegisterMetaType<fm::Time>();
	qRegisterMetaType<fm::Duration>();
	qRegisterMetaType<size_t>();
	qRegisterMetaType<TrackingVideoFrame>();

	connect(d_timer,
	        &QTimer::timeout,
	        this,
	        &TrackingVideoPlayer::onTimerTimeout);

}

TrackingVideoPlayer::~TrackingVideoPlayer() {
	d_movieThread->quit();
	d_movieThread->wait();
	d_movieThread->deleteLater();
}

void TrackingVideoPlayer::setup(IdentifiedFrameConcurrentLoader * loader) {
	d_loader =  loader;
	d_loader->setParent(nullptr);
	connect(d_movieThread,&QThread::finished,
	        d_loader,&QObject::deleteLater);
	d_loader->IdentifiedFrameConcurrentLoader::moveToThread(d_movieThread);
	connect(d_loader,
	        &IdentifiedFrameConcurrentLoader::done,
	        this,
	        &TrackingVideoPlayer::setSeekReady,
	        Qt::QueuedConnection);
	setSeekReady(d_loader->isDone());
}

bool TrackingVideoPlayer::isSeekReady() const {
	return d_seekReady;
}


TrackingVideoPlayer::State TrackingVideoPlayer::playbackState() const {
	return d_state;
}

qreal TrackingVideoPlayer::playbackRate() const {
	return d_rate;
}

fm::Duration TrackingVideoPlayer::position() const {
	return d_position;
}

fm::Duration TrackingVideoPlayer::duration() const {
	return d_duration;
}

fm::Time TrackingVideoPlayer::start() const {
	return d_start;
}

void TrackingVideoPlayer::stopTask() {
	if ( d_task == nullptr ) {
		return;
	}
	d_task->deleteLater();
	d_task = nullptr;
	d_frames.clear();
	d_stagging.clear();
	++d_currentTaskID;
}

void TrackingVideoPlayer::bootstrapTask(quint32 spaceID,
                                        const fmp::TrackingDataDirectory::Ptr & tdd) {
	if ( d_task == nullptr ) {
		return;
	}

	std::vector<TrackingVideoFrame> frames;

	for ( size_t i = 0; i < BUFFER_SIZE; ++i) {
		TrackingVideoFrame f;
		f.Image = d_task->allocate();
		frames.push_back(f);
	}

	d_task->moveToThread(d_movieThread);
	connect(d_movieThread,&QThread::finished,
	        d_task,&QObject::deleteLater);

	connect(d_task,&TrackingVideoPlayerTask::newFrame,
	        this,&TrackingVideoPlayer::onNewVideoFrame,
	        Qt::QueuedConnection);

	d_task->startLoadingFrom(spaceID,tdd);

	for ( const auto & f : frames ) {
		d_task->processNewFrame(f);
	}

}

void TrackingVideoPlayer::clearMovieSegment() {
	stop();
	stopTask();
	d_segment.reset();
	emit durationChanged(fm::Time(),0,8);
	emit positionChanged(0);
}

void TrackingVideoPlayer::setMovieSegment(quint32 spaceID,
                                          const fmp::TrackingDataDirectory::Ptr & tdd,
                                          const fmp::MovieSegment::ConstPtr & segment,
                                          const fm::Time & start) {
	if ( segment == nullptr ) {
		return;
	}

	stopTask();

	d_segment = segment;

	try {
		d_task = new TrackingVideoPlayerTask(d_currentTaskID,d_segment,computeRate(d_rate),d_loader);
		d_currentSeekID = 0;
		d_interval = fm::Duration::Second.Nanoseconds() / d_task->fps();
		d_start = start;
		d_duration = d_interval * d_task->numberOfFrame();
		emit durationChanged(start,d_duration,d_task->fps());
		d_timer->setInterval(d_interval.Milliseconds() / d_rate * computeRate(d_rate));
		d_position = 0;
		emit positionChanged(d_position);
		setSeekReady(false);
		d_displayNext = true;
	} catch ( const std::exception & e) {
		qCritical() << "Got unexpected error during loading: " << e.what();
		d_task = nullptr;
		return;
	}

	bootstrapTask(spaceID,tdd);
}

void TrackingVideoPlayer::pause() {
	if ( !d_segment || d_state != State::Playing) {
		return;
	}
	d_state = State::Paused;
	d_timer->stop();
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::play() {
	if ( !d_segment || d_state == State::Playing || d_task == nullptr) {
		return;
	}
	d_state = State::Playing;
	d_timer->start(d_interval.Milliseconds() / d_rate);
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::stop() {
	if ( !d_segment || d_state == State::Stopped ) {
		return;
	}
	d_state = State::Stopped;
	d_timer->stop();
	displayVideoFrameImpl(TrackingVideoFrame());
	emit playbackStateChanged(d_state);
}

size_t TrackingVideoPlayer::computeRate(double rate) {
	return std::max(1.0,std::floor(rate/4));
}

void TrackingVideoPlayer::setPlaybackRate(qreal rate) {
	if ( rate == d_rate || rate <= 0.0 ) {
		return;
	}
	d_rate = rate;
	auto pRate = computeRate(d_rate);
	if ( d_interval.Nanoseconds() != 0 ) {
		d_timer->setInterval(d_interval.Milliseconds() / d_rate * pRate);
	}
	if ( d_task != nullptr ) {
		d_task->setRate(pRate);
	}
	emit playbackRateChanged(rate);
}

void TrackingVideoPlayer::setPosition(fm::Duration position) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[Player][setPosition]: Thread is " << QThread::currentThread() << " myself is " << thread() << std::endl);
	position = std::clamp(position,fm::Duration(0),d_duration);

	if ( d_task == nullptr || position == d_position ) {
		return;
	}
	d_task->seek(++d_currentSeekID,position);
	VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Seeked to " << position << " seekID is " << d_currentSeekID << std::endl);

	d_displayNext = true;
	for ( auto & f : d_frames ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Resending buffered frame " << f << std::endl);
		d_task->processNewFrame(f);
	}
	d_frames.clear();

	for ( auto & f : d_stagging ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Resending stagged frame " << f << std::endl);
		d_task->processNewFrame(f);
	}
	d_stagging.clear();

}

void TrackingVideoPlayer::onNewVideoFrame(size_t taskID, size_t seekID, TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "[Player][onNewVideoFrame]: Thread is " << QThread::currentThread() << " myself is " << thread() << std::endl;
			std::cerr << "[Player][onNewVideoFrame]: Received from Task:" << taskID << " from seek " << seekID << " frame " << frame << std::endl;
		});

	if ( taskID != d_currentTaskID ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Mismatching taskID "<< taskID << " (expected:" << d_currentTaskID << "): forgetting frame" << std::endl);
		// this videoframe is from a dead task
		return;
	}

	if ( seekID != d_currentSeekID ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Mismatching seekID " << seekID << " (expected:" << d_currentSeekID << ")"<< std::endl;);
		if ( d_scrollMode == true ) {
			VIDEO_PLAYER_DEBUG(std::cerr << "[Player] in scroll mode, so displaying current frame anyway"  << frame << std::endl);
			displayVideoFrameImpl(frame);
		} else {
			d_task->processNewFrame(frame);
		}
		return;
	}

	if ( frame.FrameID == std::numeric_limits<fmp::MovieFrameID>::max() ) {
		// no frame case at end of file.
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] EOF: stagging frame " << frame << std::endl);
		d_stagging.push_back(frame);
		return;
	}


	if ( d_displayNext == false && frame.EndPos < d_position ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Discarding old frame " << frame << std::endl);
		// already old, we discard it
		d_task->processNewFrame(frame);
		return;
	}



	if (d_displayNext == true ) {
		if ( d_position == frame.StartPos ) {
			// avoids forward deadlock while jumping next frame, can
			// happen depending on OpenCV implementation.
			VIDEO_PLAYER_DEBUG(std::cerr << "Frame did not advance forward, releasing it" << std::endl);
			d_task->processNewFrame(frame);
			return;
		}
		VIDEO_PLAYER_DEBUG(std::cerr << "Displaying first frame " << frame << " size: " << d_frames.size() << std::endl);

		d_displayNext = false;
		d_position = frame.StartPos;
		displayVideoFrameImpl(frame);
		if ( d_scrollMode == false ) {
			emit positionChanged(d_position);
		}
		return;
	}

	VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Adding frame to queue" << frame << std::endl);
	d_frames.push_back(frame);
}

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
void printFrames (const std::deque<TrackingVideoFrame> & frames ) {
	std::cerr << "[Player] Frames:{";
	auto prefix = "";
	for ( const auto & f : frames ) {
		std::cerr << prefix << f;
		prefix = ",";
	}
	std::cerr << "}" << std::endl;
}
#endif

void TrackingVideoPlayer::onTimerTimeout() {
	d_position = d_position + d_interval * computeRate(d_rate);
	VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Current position is " << d_position << std::endl);
	emit positionChanged(d_position);

	VIDEO_PLAYER_DEBUG({
			std::cerr << "[Player] === BEFORE SORT ===" << std::endl;
			printFrames(d_frames);
		});

	std::sort(d_frames.begin(),
	          d_frames.end(),
	          [](const TrackingVideoFrame & a, const TrackingVideoFrame & b) {
		          return a.StartPos < b.StartPos;
	          });

	std::deque<TrackingVideoFrame> deleted;
	auto last = std::remove_if(d_frames.begin(),
	                           d_frames.end(),
	                           [this,&deleted](const TrackingVideoFrame & a) {
		                           if ( a.EndPos <= d_position ) {
			                           deleted.push_back(a);
			                           return true;
		                           }
		                           return false;
	                           });

	VIDEO_PLAYER_DEBUG({
			std::cerr << "[Player] === BEFORE REMOVE ===" << std::endl;
			printFrames(d_frames);
			std::cerr << "[Player] === WILL BE REMOVED ===" << std::endl;
			printFrames(deleted);
		});
	d_frames.erase(last,d_frames.end());

	if ( d_frames.empty() ==  true
	     && deleted.empty() == false
	     && computeRate(d_rate) > 1 ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] reset time to " << deleted.back().StartPos << std::endl);
		d_position = deleted.back().StartPos;
	}


	//removes expired frames
	for ( const auto & f: deleted ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Will release " << f << std::endl);
		if ( d_task != nullptr ) {
			d_task->processNewFrame(f);
		}
	}

	VIDEO_PLAYER_DEBUG({
			std::cerr << "[Player] === AFTER REMOVE ===" << std::endl;
			printFrames(d_frames);
		});

	if ( d_frames.empty() == true ) {
		if ( d_stagging.empty() == false ) {
			stop();
		}
		return;
	}

	if ( d_frames.front().StartPos > d_position ) {
		return;
	}

	displayVideoFrameImpl(d_frames.front());
	d_frames.pop_front();
}


void TrackingVideoPlayer::displayVideoFrameImpl(const TrackingVideoFrame & frame) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Displaying frame:" << frame << std::endl);

	if (!d_displayed.Image == false && d_task != nullptr) {
		VIDEO_PLAYER_DEBUG(std::cerr << "[Player] Releasing " << d_displayed << std::endl);
		d_task->processNewFrame(d_displayed);
	}
	d_displayed = frame;
	emit displayVideoFrame(d_displayed);
}

void TrackingVideoPlayer::togglePlayPause() {
	switch(d_state) {
	case State::Playing:
		pause();
		break;
	case State::Paused:
	case State::Stopped:
		play();
		break;
	}
}

void TrackingVideoPlayer::jumpNextFrame() {
	if ( d_state != State::Paused) {
		return;
	}
	if ( d_rate != 1.0 ) {
		setPlaybackRate(1.0);
		setPosition(d_position + d_interval);
	} else {
		onTimerTimeout();
	}
}

void TrackingVideoPlayer::jumpPrevFrame() {
	if ( d_state != State::Paused || d_displayNext == true) {
		return;
	}
	if ( d_rate != 1.0 ) {
		setPlaybackRate(1.0);
	}
#if CV_MAJOR_VERSION == 4
	setPosition(d_position - fm::Duration::Millisecond);
#else
	setPosition(d_position - d_interval);
#endif
}

void TrackingVideoPlayer::skipDuration(fm::Duration duration) {
	if ( d_task == nullptr ) {
		return;
	}
	setPosition(d_position + duration);
}

void TrackingVideoPlayer::setTime(const fm::Time & time) {
	if ( d_task == nullptr ) {
		return;
	}
	auto actualTime = std::clamp(time,d_start,d_start.Add(d_duration));
	setPosition(time.Sub(d_start));
}

bool TrackingVideoPlayer::scrollMode() const {
	return d_scrollMode;
}

void TrackingVideoPlayer::setScrollMode(bool scrollMode) {
	d_scrollMode = scrollMode;
}

const fmp::MovieSegment::ConstPtr & TrackingVideoPlayer::currentSegment() const {
	return d_segment;
}


void TrackingVideoPlayer::setSeekReady(bool ready) {
	if ( ready == d_seekReady) {
		return;
	}
	d_seekReady = ready;
	emit seekReady(d_seekReady);
}


void TrackingVideoPlayer::jumpNextVisible(fmp::AntID antID, bool backward) {
	if ( d_task == nullptr
	     || d_seekReady == false
	     || d_displayed.Contains(antID) == true ) {
		return;
	}

	quint64 frame;

	metaObject()->invokeMethod(d_loader,"findAnt",Qt::BlockingQueuedConnection,
	                           Q_RETURN_ARG(quint64,frame),
	                           Q_ARG(quint32,antID),
	                           Q_ARG(quint64,d_displayed.FrameID),
	                           Q_ARG(int,backward == true ? -1 : 1));

	if ( frame > d_segment->EndMovieFrame() ) {
		return;
	}
	setPosition(d_interval * (frame-d_segment->StartMovieFrame()));
}

TrackingVideoPlayerTask::TrackingVideoPlayerTask(size_t taskID,
                                                 const fmp::MovieSegment::ConstPtr & segment,
                                                 size_t rate,
                                                 IdentifiedFrameConcurrentLoader * loader)
	: QObject(nullptr)
	, d_segment(segment)
	, d_capture(segment->AbsoluteFilePath().c_str())
	, d_loader(loader)
	, d_taskID(taskID)
	, d_seekID(0)
	, d_rate(std::max(size_t(1),rate)) {
	if ( d_capture.isOpened() == false ) {
		throw std::runtime_error("Could not open '" + segment->AbsoluteFilePath().string() + "'");
	}
	d_width = d_capture.get(cv::CAP_PROP_FRAME_WIDTH);
	d_height = d_capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	d_expectedFrameDuration = double(fm::Duration::Second.Nanoseconds()) / d_capture.get(cv::CAP_PROP_FPS);
}

TrackingVideoPlayerTask::~TrackingVideoPlayerTask() {
}

double TrackingVideoPlayerTask::fps() const {
	return d_capture.get(cv::CAP_PROP_FPS);
}

qint64 TrackingVideoPlayerTask::numberOfFrame() const {
	return d_capture.get(cv::CAP_PROP_FRAME_COUNT);
}

void TrackingVideoPlayerTask::processNewFrame(TrackingVideoFrame frame) {
	metaObject()->invokeMethod(this,"processNewFrameUnsafe",Qt::QueuedConnection,
	                           Q_ARG(TrackingVideoFrame,frame));
}

void TrackingVideoPlayerTask::seek(size_t seekID, fm::Duration position) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "[task] seek to " << seekID << " at " << position << std::endl;
			std::cerr << "[task] Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	metaObject()->invokeMethod(this,"seekUnsafe",Qt::BlockingQueuedConnection,
	                           Q_ARG(size_t,seekID),
	                           Q_ARG(fm::Duration,position));
}

void TrackingVideoPlayerTask::setRate(size_t rate) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "[task] set Rate: " << rate << std::endl;
		});
	metaObject()->invokeMethod(this,"setRateUnsafe",Qt::BlockingQueuedConnection,
	                           Q_ARG(size_t,rate));
}

void TrackingVideoPlayerTask::setRateUnsafe(size_t rate) {
	d_rate = std::max(rate,size_t(1));
}

void TrackingVideoPlayerTask::processNewFrameUnsafe(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "[task] Processing Image " << frame.Image.get() << std::endl;
			std::cerr << "[task] Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});

	for ( size_t i = 0; i < d_rate; ++i ) {
		frame.FrameID = d_capture.get(cv::CAP_PROP_POS_FRAMES);
		if ( d_capture.grab() == false ) {
			VIDEO_PLAYER_DEBUG(std::cerr << "[task] Could not capture Image " << frame.Image.get() << std::endl);

			frame.FrameID = std::numeric_limits<fmp::MovieFrameID>::max();
			emit newFrame(d_taskID,d_seekID,frame);
			return;
		}
	}

	cv::Mat mat(d_height,d_width,CV_8UC3,const_cast<uchar*>(frame.Image->constBits()),frame.Image->bytesPerLine());
	d_capture.retrieve(mat);

	frame.EndPos = d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond;
	frame.StartPos = frame.EndPos - d_expectedFrameDuration;


	VIDEO_PLAYER_DEBUG({
			std::cerr << "[task] emitting new Frame Image " << frame << " on seek " << d_seekID << std::endl;
			std::cerr << "[task] Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	frame.TrackingFrame = d_loader->frameAt(frame.FrameID);
	frame.CollisionFrame = d_loader->collisionAt(frame.FrameID);
	emit newFrame(d_taskID,d_seekID,frame);
}

std::shared_ptr<QImage> TrackingVideoPlayerTask::allocate() const {
	return std::make_shared<QImage>(d_width,d_height,QImage::Format_RGB888);
}


void TrackingVideoPlayerTask::seekUnsafe(size_t seekID, fm::Duration position) {
	VIDEO_PLAYER_DEBUG({
		std::cerr << "[task] seek unsafe to " << seekID << " at " << position << std::endl;
		std::cerr << "[task] Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	d_seekID = seekID;
	if ( d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond == position ) {
		return;
	}
	d_capture.set(cv::CAP_PROP_POS_MSEC,qint64(position.Milliseconds()));
};

void TrackingVideoPlayerTask::startLoadingFrom(quint32 spaceID,
                                               const fmp::TrackingDataDirectory::Ptr & tdd) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[task] startLoadingFrom" << std::endl);
	metaObject()->invokeMethod(this,"startLoadingFromUnsafe",Qt::BlockingQueuedConnection,
	                           Q_ARG(quint32,spaceID),
	                           Q_ARG(fmp::TrackingDataDirectory::Ptr,tdd));
}

void TrackingVideoPlayerTask::startLoadingFromUnsafe(quint32 spaceID,
                                                     fmp::TrackingDataDirectory::Ptr tdd) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[task] startLoadingFromUnsafe" << std::endl);
	d_loader->loadMovieSegment(spaceID,tdd, d_segment);
}
