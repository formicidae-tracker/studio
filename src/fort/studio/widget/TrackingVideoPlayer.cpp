#include "TrackingVideoPlayer.hpp"


#include <QDebug>

#include <fort/studio/Format.hpp>

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QThread>

#include "TrackingVideoWidget.hpp"

TrackingVideoPlayer::TrackingVideoPlayer(QObject * parent)
	: QObject(parent)
	, d_task(nullptr)
	, d_state(State::Stopped)
	, d_rate(1.0)
	, d_widget(nullptr)
	, d_movieThread(new QThread()) {
	d_movieThread->start();
	qRegisterMetaType<TrackingVideoFrame>();
}

TrackingVideoPlayer::~TrackingVideoPlayer() {
	stopAndWaitTask();
	d_movieThread->quit();
	d_movieThread->wait();
	d_movieThread->deleteLater();
}

void TrackingVideoPlayer::setVideoOutput(TrackingVideoWidget * widget) {
	if ( d_widget != nullptr ) {
		qCritical() << "Only supports one output";
	}
	d_widget = widget;
	connect(this,
	        &TrackingVideoPlayer::displayVideoFrame,
	        d_widget,
	        &TrackingVideoWidget::display);


}

TrackingVideoPlayer::State TrackingVideoPlayer::playbackState() const {
	return d_state;
}

qreal TrackingVideoPlayer::playbackRate() const {
	return d_rate;
}



qint64 TrackingVideoPlayer::position() const {
	return 0;
}


void TrackingVideoPlayer::setMovieSegment(const fmp::MovieSegment::ConstPtr & segment,
                                          const fm::Time & start,
                                          const fm::Time & end) {
	if ( d_widget == nullptr ) {
		return;
	}

	stopAndWaitTask();

	d_segment = segment;
	emit durationChanged(start,end.Sub(start).Nanoseconds());

	try {
		d_task = new TrackingVideoPlayerTask(d_segment);
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
	for ( const auto & [time,frame] : d_frames ) {
		emit frameDone(frame.Image);
	}
	d_frames.clear();
	d_task->waitFinished();
}

void TrackingVideoPlayer::pause() {
	if ( !d_segment || d_state != State::Playing) {
		return;
	}
	d_state = State::Paused;
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::play() {
	if ( !d_segment || d_state == State::Playing ) {
		return;
	}
	d_state = State::Playing;
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::stop() {
	if ( !d_segment || d_state != State::Playing ) {
		return;
	}
	d_state = State::Stopped;
	emit playbackStateChanged(d_state);
}


void TrackingVideoPlayer::setPlaybackRate(qreal rate) {
	if ( rate == d_rate || rate <= 0.0 ) {
		return;
	}
	d_rate = rate;
	emit playbackRateChanged(rate);
}

void TrackingVideoPlayer::setPosition(qint64 nanoseconds) {

}

void TrackingVideoPlayer::onNewVideoFrame(TrackingVideoFrame frame) {
	qWarning() << "Got frame " << frame.FrameID << " at " << ToQString(fm::Duration(frame.StartMS * fm::Duration::Millisecond));
	d_frames.insert(std::make_pair(frame.StartMS * fm::Duration::Millisecond,frame));
	emit displayVideoFrame(*(--d_frames.end())->second.Image);
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

void TrackingVideoPlayerTask::startOn(QThread * thread) {
	connect(thread,&QThread::finished,
	        this,&QObject::deleteLater);
	this->moveToThread(thread);
	metaObject()->invokeMethod(this,"start",Qt::QueuedConnection);
}

TrackingVideoPlayerTask::~TrackingVideoPlayerTask() {
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


void TrackingVideoPlayerTask::onReleasedImage(QImage *image) {
	d_waitingImages.erase(image);

	TrackingVideoFrame frame;
	frame.FrameID = d_capture.get(cv::CAP_PROP_POS_FRAMES);
	frame.StartMS = d_capture.get(cv::CAP_PROP_POS_MSEC);

	if ( d_stopped.load() == true || d_capture.grab() == false ) {
		if ( d_waitingImages.empty() ) {
			emit finished();
		}
		return;
	}

	frame.Image = image;

	cv::Mat mat(d_height,d_width,CV_8UC3,const_cast<uchar*>(image->constBits()),image->bytesPerLine());
	d_capture.retrieve(mat);

	frame.EndMS = d_capture.get(cv::CAP_PROP_POS_MSEC);

	d_waitingImages.insert(image);
	emit newFrame(frame);
}


void TrackingVideoPlayerTask::waitFinished() {
	std::unique_lock<std::mutex> lock(d_mutex);
	d_condition.wait(lock,[this]() { return d_done; });
}

void TrackingVideoPlayerTask::stop() {
	d_stopped.store(true);
}


void TrackingVideoPlayerTask::seek(quint64 pos) {
};
