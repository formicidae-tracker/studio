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
	, d_seekReady(true) {
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
}

void TrackingVideoPlayer::bootstrapTask(const fmp::TrackingDataDirectory::ConstPtr & tdd) {
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

	d_task->startLoadingFrom(tdd);

	for ( const auto & f : frames ) {
		d_task->processNewFrame(f);
	}

}

void TrackingVideoPlayer::setMovieSegment(const fmp::TrackingDataDirectory::ConstPtr & tdd,
                                          const fmp::MovieSegment::ConstPtr & segment,
                                          const fm::Time & start) {
	if ( !segment ) {
		return;
	}

	stopTask();

	d_segment = segment;

	try {
		d_task = new TrackingVideoPlayerTask(++d_currentTaskID,d_segment,d_loader);
		d_currentSeekID = 0;
		d_interval = fm::Duration::Second.Nanoseconds() / d_task->fps();
		d_start = start;
		d_duration = d_interval * d_task->numberOfFrame();
		emit durationChanged(start,d_duration);
		d_timer->setInterval(d_interval.Milliseconds() / d_rate);
		d_position = 0;
		emit positionChanged(d_position);
		setSeekReady(false);
		d_displayNext = true;
	} catch ( const std::exception & e) {
		qCritical() << "Got unexpected error: " << e.what();
		d_task = nullptr;
		return;
	}

	bootstrapTask(tdd);
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
	emit displayVideoFrame(TrackingVideoFrame());
	emit playbackStateChanged(d_state);
}


void TrackingVideoPlayer::setPlaybackRate(qreal rate) {
	if ( rate == d_rate || rate <= 0.0 ) {
		return;
	}
	d_rate = rate;
	if ( d_interval.Nanoseconds() != 0 ) {
		d_timer->setInterval(d_interval.Milliseconds() / d_rate);
	}
	emit playbackRateChanged(rate);
}

void TrackingVideoPlayer::setPosition(fm::Duration position) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[setPosition]: Thread is " << QThread::currentThread() << " myself is " << thread() << std::endl);

	if ( d_task == nullptr ) {
		return;
	}
	d_task->seek(++d_currentSeekID,position);
	VIDEO_PLAYER_DEBUG(std::cerr << "Seeked to " << position << " seekID is " << d_currentSeekID << std::endl);

	d_displayNext = true;
	for ( auto & f : d_frames ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Resending buffered frame " << f << std::endl);
		d_task->processNewFrame(f);
	}
	d_frames.clear();

	for ( auto & f : d_stagging ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Resending stagged frame " << f << std::endl);
		d_task->processNewFrame(f);
	}
	d_stagging.clear();

}

void TrackingVideoPlayer::onNewVideoFrame(size_t taskID, size_t seekID, TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "[onNewVideoFrame]: Thread is " << QThread::currentThread() << " myself is " << thread() << std::endl;
			std::cerr << "Received from Task:" << taskID << " from seek " << seekID << " frame " << frame << std::endl;
		});

	if ( taskID != d_currentTaskID ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Mismatching taskID "<< taskID << " (expected:" << d_currentTaskID << "): forgetting frame" << std::endl);
		// this videoframe is from a dead task
		return;
	}

	if ( seekID != d_currentSeekID ) {
		emit displayVideoFrame(frame);
		// we have seeked... reprocess the frame immediatly
		if ( d_task != nullptr ) {
			VIDEO_PLAYER_DEBUG(std::cerr << "Mismatching seekID " << seekID << " (expected:" << d_currentSeekID << "):  reprocessing frame "  << frame << std::endl);
			d_task->processNewFrame(frame);
		}
		return;
	}

	if ( frame.FrameID == std::numeric_limits<fmp::MovieFrameID>::max() ) {
		// no frame case at end of file.
		VIDEO_PLAYER_DEBUG(std::cerr << "EOF: stagging frame " << frame << std::endl);
		d_stagging.push_back(frame);
		return;
	}

	VIDEO_PLAYER_DEBUG(std::cerr << "Pushing frame " << frame << std::endl);

	if ( d_displayNext == false && frame.EndPos < d_position ) {
		// already old, we discard it
		d_task->processNewFrame(frame);
		return;
	}


	d_frames.push_back(frame);
	if (d_displayNext == true ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Displaying first frame " << frame << " size: " << d_frames.size() << std::endl);
		d_displayNext = false;
		d_position = d_frames.back().StartPos;
		emit displayVideoFrame(d_frames.back());

	}
}

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
void printFrames (const std::vector<TrackingVideoFrame> & frames ) {
	std::cerr << "Frames:{";
	auto prefix = "";
	for ( const auto & f : frames ) {
		std::cerr << prefix << f;
		prefix = ",";
	}
	std::cerr << "}" << std::endl;
}
#endif

void TrackingVideoPlayer::onTimerTimeout() {
	d_position = d_position + d_interval;
	VIDEO_PLAYER_DEBUG(std::cerr << "Current position is " << d_position << std::endl);
	emit positionChanged(d_position);

	VIDEO_PLAYER_DEBUG({
			std::cerr << "=== BEFORE SORT ===" << std::endl;
			printFrames(d_frames);
		});

	std::sort(d_frames.begin(),
	          d_frames.end(),
	          [](const TrackingVideoFrame & a, const TrackingVideoFrame & b) {
		          return a.StartPos < b.StartPos;
	          });

	std::vector<TrackingVideoFrame> deleted;
	deleted.reserve(d_frames.size());
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
			std::cerr << "=== BEFORE REMOVE ===" << std::endl;
			printFrames(d_frames);
			std::cerr << "=== WILL BE REMOVED ===" << std::endl;
			printFrames(deleted);
		});
	d_frames.erase(last,d_frames.end());

	//removes expired frames
	for ( const auto & f: deleted ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Will release " << f << std::endl);
		if ( d_task != nullptr ) {
			d_task->processNewFrame(f);
		}
	}

	VIDEO_PLAYER_DEBUG({
			std::cerr << "=== AFTER REMOVE ===" << std::endl;
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

	VIDEO_PLAYER_DEBUG(std::cerr << "Emitting frame:" << d_frames.front() << std::endl);
	emit displayVideoFrame(d_frames.front());
}


TrackingVideoPlayerTask::TrackingVideoPlayerTask(size_t taskID,
                                                 const fmp::MovieSegment::ConstPtr & segment,
                                                 IdentifiedFrameConcurrentLoader * loader)
	: QObject(nullptr)
	, d_segment(segment)
	, d_capture(segment->AbsoluteFilePath().c_str())
	, d_loader(loader)
	, d_taskID(taskID)
	, d_seekID(0) {
	if ( d_capture.isOpened() == false ) {
		throw std::runtime_error("Could not open '" + segment->AbsoluteFilePath().string() + "'");
	}

	d_width = d_capture.get(cv::CAP_PROP_FRAME_WIDTH);
	d_height = d_capture.get(cv::CAP_PROP_FRAME_HEIGHT);

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
			std::cerr << "seek to " << seekID << " at " << position << std::endl;
			std::cerr << "Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	metaObject()->invokeMethod(this,"seekUnsafe",Qt::BlockingQueuedConnection,
	                           Q_ARG(size_t,seekID),
	                           Q_ARG(fm::Duration,position));
}

void TrackingVideoPlayerTask::processNewFrameUnsafe(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG({
			std::cerr << "Processing Image " << frame.Image.get() << std::endl;
			std::cerr << "Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});

	frame.FrameID = d_capture.get(cv::CAP_PROP_POS_FRAMES);
	frame.StartPos = d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond;

	if ( d_capture.grab() == false ) {
		VIDEO_PLAYER_DEBUG(std::cerr << "Could not capture Image " << frame.Image.get() << std::endl);

		frame.FrameID = std::numeric_limits<fmp::MovieFrameID>::max();
		emit newFrame(d_taskID,d_seekID,frame);
		return;
	}

	cv::Mat mat(d_height,d_width,CV_8UC3,const_cast<uchar*>(frame.Image->constBits()),frame.Image->bytesPerLine());
	d_capture.retrieve(mat);

	frame.EndPos = d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond;

	VIDEO_PLAYER_DEBUG({
			std::cerr << "emitting new Frame Image " << frame << " on seek " << d_seekID << std::endl;
			std::cerr << "Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	frame.TrackingFrame = d_loader->FrameAt(frame.FrameID);

	emit newFrame(d_taskID,d_seekID,frame);
}

std::shared_ptr<QImage> TrackingVideoPlayerTask::allocate() const {
	return std::make_shared<QImage>(d_width,d_height,QImage::Format_RGB888);
}


void TrackingVideoPlayerTask::seekUnsafe(size_t seekID, fm::Duration position) {
	VIDEO_PLAYER_DEBUG({
		std::cerr << "seek unsafe to " << seekID << " at " << position << std::endl;
		std::cerr << "Current thread: " << QThread::currentThread() << " my thread: " << thread() << std::endl;
		});
	d_seekID = seekID;
	d_capture.set(cv::CAP_PROP_POS_MSEC,qint64(position.Milliseconds()));
};

void TrackingVideoPlayerTask::startLoadingFrom(const fmp::TrackingDataDirectory::ConstPtr & tdd) {
	VIDEO_PLAYER_DEBUG(std::cerr << "startLoadingFrom" << std::endl);
	metaObject()->invokeMethod(this,"startLoadingFromUnsafe",Qt::BlockingQueuedConnection,
	                           Q_ARG(fmp::TrackingDataDirectory::ConstPtr,tdd));
}

void TrackingVideoPlayerTask::startLoadingFromUnsafe(fmp::TrackingDataDirectory::ConstPtr tdd) {
	VIDEO_PLAYER_DEBUG(std::cerr << "startLoadingFromUnsafe" << std::endl);
	d_loader->loadMovieSegment(tdd, d_segment);
}

void TrackingVideoPlayer::setSeekReady(bool ready) {
	if ( ready == d_seekReady) {
		return;
	}
	d_seekReady = ready;
	emit seekReady(d_seekReady);
}
