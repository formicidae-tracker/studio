#include "TrackingVideoPlayer.hpp"


#include <QDebug>
#include <QTimer>
#include <QThread>


#include <fort/studio/Format.hpp>


#include "TrackingVideoWidget.hpp"

TrackingVideoPlayer::TrackingVideoPlayer(QObject * parent)
	: QObject(parent)
	, d_task(nullptr)
	, d_state(State::Stopped)
	, d_rate(1.0)
	, d_movieThread(new QThread())
	, d_timer(new QTimer(this)) {
	d_movieThread->start();
	qRegisterMetaType<fm::Time>();
	qRegisterMetaType<fm::Duration>();
	qRegisterMetaType<TrackingVideoFrame>();

	connect(d_timer,
	        &QTimer::timeout,
	        this,
	        &TrackingVideoPlayer::onTimerTimeout);
}

TrackingVideoPlayer::~TrackingVideoPlayer() {
	stopAndWaitTask();
	d_movieThread->quit();
	d_movieThread->wait();
	d_movieThread->deleteLater();
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

std::mutex TrackingVideoFrame::debugMutex;

void TrackingVideoPlayer::setMovieSegment(const fmp::MovieSegment::ConstPtr & segment,
                                          const fm::Time & start) {
	stopAndWaitTask();

	d_segment = segment;

	try {
		d_task = new TrackingVideoPlayerTask(d_segment);
		d_interval = fm::Duration::Second.Nanoseconds() / d_task->fps();
		d_start = start;
		d_duration = d_interval * d_task->numberOfFrame();
		emit durationChanged(start,d_duration);
		d_timer->setInterval(d_interval.Milliseconds() * d_rate);
		d_position = 0;
		d_displayNext = true;
	} catch ( const std::exception & e) {
		qCritical() << "Got unexpected error: " << e.what();
		d_task = nullptr;
		return;
	}
	connect(d_task,&TrackingVideoPlayerTask::newFrame,
	        this,&TrackingVideoPlayer::onNewVideoFrame);
	connect(this,&TrackingVideoPlayer::frameDone,
	        d_task,&TrackingVideoPlayerTask::onReleasedImage);
	d_task->startOn(d_movieThread);
}

void TrackingVideoPlayer::stopAndWaitTask() {
	if ( d_task == nullptr ) {
		return;
	}
	d_task->stop();
	for ( const auto & frame : d_frames ) {
		emit frameDone(frame.Image);
	}
	d_frames.clear();
	d_task->waitDone();
	delete d_task;
	d_task = nullptr;
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
	d_timer->start(d_interval.Milliseconds() * d_rate);
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::stop() {
	if ( !d_segment || d_state != State::Playing ) {
		return;
	}
	d_state = State::Stopped;
	d_timer->stop();
	emit playbackStateChanged(d_state);
}


void TrackingVideoPlayer::setPlaybackRate(qreal rate) {
	if ( rate == d_rate || rate <= 0.0 ) {
		return;
	}
	d_rate = rate;
	if ( d_interval.Nanoseconds() != 0 ) {
		d_timer->setInterval(d_interval.Milliseconds() * d_rate);
	}
	emit playbackRateChanged(rate);
}

void TrackingVideoPlayer::setPosition(fm::Duration position) {
	if ( d_task == nullptr ) {
		return;
	}
	std::cerr << "Want position " << position << std::endl;
}

void TrackingVideoPlayer::onNewVideoFrame(TrackingVideoFrame frame) {
	{
			std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
			std::cerr << "New frame " << frame << std::endl;
	}
	d_frames.push_back(frame);
	if (d_displayNext == true ) {
		d_displayNext = false;
		d_position = d_frames.back().StartPos;
		emit positionChanged(d_position);
		emit displayVideoFrame(d_frames.back());

	}
}


void printFrames (const std::vector<TrackingVideoFrame> & frames ) {

	std::cerr << "Frames:{";
	auto prefix = "";
	for ( const auto & f : frames ) {
		std::cerr << prefix << f;
		prefix = ",";
	}
	std::cerr << "}" << std::endl;
}

void TrackingVideoPlayer::onTimerTimeout() {
	d_position = d_position + d_interval;
	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "Current position is " << d_position << std::endl;
	}
	emit positionChanged(d_position);

	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "=== BEFORE SORT ===" << std::endl;
		printFrames(d_frames);
	}



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

	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "=== BEFORE REMOVE ===" << std::endl;
		printFrames(d_frames);
		std::cerr << "=== WILL BE REMOVED ===" << std::endl;
		printFrames(deleted);
	}
	d_frames.erase(last,d_frames.end());

	//removes expired frames
	for ( const auto & f: deleted ) {
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "Will release " << f << std::endl;
		emit frameDone(f.Image);
	}

	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "=== AFTER REMOVE ===" << std::endl;
		printFrames(d_frames);
	}

	if ( d_frames.empty() == true ) {
		if ( d_task != nullptr && d_task->isDone() == true ) {
			TrackingVideoFrame blankFrame;
			blankFrame.Image = nullptr;
			emit displayVideoFrame(blankFrame);
			stop();
		}
		return;
	}

	if ( d_frames.front().StartPos > d_position ) {
		return;
	}
	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "Emitting frame:" << d_frames.front() << std::endl;
	}
	emit displayVideoFrame(d_frames.front());
}


TrackingVideoPlayerTask::TrackingVideoPlayerTask(const fmp::MovieSegment::ConstPtr & segment)
	: QObject(nullptr)
	, d_capture(segment->AbsoluteFilePath().c_str())
	, d_done(false) {
	if ( d_capture.isOpened() == false ) {
		throw std::runtime_error("Could not open '" + segment->AbsoluteFilePath().string() + "'");
	}

	d_width = d_capture.get(cv::CAP_PROP_FRAME_WIDTH);
	d_height = d_capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	connect(this, &TrackingVideoPlayerTask::finished,
	        [this]() {
		        std::lock_guard<std::mutex> lock(d_mutex);
		        d_done = true;
		        d_condition.notify_all();
	        });

}

double TrackingVideoPlayerTask::fps() const {
	return d_capture.get(cv::CAP_PROP_FPS);
}

qint64 TrackingVideoPlayerTask::numberOfFrame() const {
	return d_capture.get(cv::CAP_PROP_FRAME_COUNT);
}

void TrackingVideoPlayerTask::startOn(QThread * thread) {
	connect(thread,&QThread::finished,
	        this,&QObject::deleteLater);
	this->moveToThread(thread);
	metaObject()->invokeMethod(this,"start",Qt::QueuedConnection);
}

TrackingVideoPlayerTask::~TrackingVideoPlayerTask() {
}



bool TrackingVideoPlayerTask::isDone() {
	std::lock_guard<std::mutex> lock(d_mutex);
	return d_done;
}

void TrackingVideoPlayerTask::start() {
	for ( size_t i = 0; i < 3; ++i ) {
		d_images.push_back(QImage(d_width,d_height,QImage::Format_RGB888));
	}

	d_stopped.store(false);

	for (auto & image : d_images) {
		onReleasedImage(&image);
	}
}

void printImages(const std::set<QImage*> images) {
	std::cerr << "images: {";
	auto prefix  = "";
	for ( const auto i : images ) {
		std::cerr << prefix << (void*)i;
		prefix = ",";
	}
	std::cerr << "}" << std::endl;
}
void TrackingVideoPlayerTask::onReleasedImage(QImage *image) {
	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		std::cerr << "Released " << image << std::endl;
		printImages(d_waitingImages);
	}
	d_waitingImages.erase(image);
	{
		std::lock_guard<std::mutex> lock(TrackingVideoFrame::debugMutex);
		printImages(d_waitingImages);
	}
	TrackingVideoFrame frame;
	frame.FrameID = d_capture.get(cv::CAP_PROP_POS_FRAMES);
	frame.StartPos = d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond;

	if ( d_stopped.load() == true || d_capture.grab() == false ) {
		if ( d_waitingImages.empty() ) {
			emit finished();
		}
		return;
	}

	frame.Image = image;

	cv::Mat mat(d_height,d_width,CV_8UC3,const_cast<uchar*>(image->constBits()),image->bytesPerLine());
	d_capture.retrieve(mat);

	frame.EndPos = d_capture.get(cv::CAP_PROP_POS_MSEC) * fm::Duration::Millisecond;

	d_waitingImages.insert(image);
	emit newFrame(frame);
}


void TrackingVideoPlayerTask::waitDone() {
	std::unique_lock<std::mutex> lock(d_mutex);
	d_condition.wait(lock,[this]() { return d_done; });
}

void TrackingVideoPlayerTask::stop() {
	d_stopped.store(true);
}


void TrackingVideoPlayerTask::seek(quint64 pos) {
};
