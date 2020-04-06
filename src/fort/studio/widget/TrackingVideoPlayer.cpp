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
		d_task = new TrackingVideoPlayerTask(d_segment->AbsoluteFilePath().c_str(),
		                                     d_widget->context());
	} catch ( const std::exception & e) {
		qCritical() << "Got unexpected error: " << e.what();
		d_task = nullptr;
		return;
	}
	connect(d_task,&TrackingVideoPlayerTask::newFrame,
	        this,&TrackingVideoPlayer::onNewVideoFrame);
	connect(this,&TrackingVideoPlayer::frameDone,
	        d_task,&TrackingVideoPlayerTask::onReleasedBuffer);
	d_task->startOn(d_movieThread);
}

void TrackingVideoPlayer::stopAndWaitTask() {
	if ( d_task == nullptr ) {
		return;
	}
	d_task->stop();
	for ( const auto & [time,frame] : d_frames ) {
		emit frameDone(frame.Buffer);
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
	qWarning() << "Got frame " << frame.FrameID << " at " << ToQString(fm::Duration(frame.PositionMS * fm::Duration::Millisecond));
	d_frames.insert(std::make_pair(frame.PositionMS * 1e6,frame));
	emit displayVideoFrame(&((--d_frames.end())->second));
}



TrackingVideoPlayerTask::TrackingVideoPlayerTask(const QString & path,
                                                 QOpenGLContext * sharedContext)
	: QObject(nullptr)
	, d_capture(ToStdString(path))
	, d_context(new QOpenGLContext(this))
	, d_surface(new QOffscreenSurface(nullptr))
	, d_done(false) {
	if ( d_capture.isOpened() == false ) {
		throw std::runtime_error("Could not open '" + ToStdString(path) + "'");
	}

	d_surface->setFormat(sharedContext->format());
	d_surface->create();

	d_context->setFormat(sharedContext->format());
	d_context->setShareContext(sharedContext);
	d_context->create();

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
	d_context->moveToThread(thread);
	metaObject()->invokeMethod(this,"start",Qt::QueuedConnection);
}

TrackingVideoPlayerTask::~TrackingVideoPlayerTask() {
	delete d_surface;
}

void TrackingVideoPlayerTask::start() {
	d_context->makeCurrent(d_surface);
	for ( size_t i = 0; i < 3; ++i ) {
		d_PBOs.push_back(QOpenGLBuffer(QOpenGLBuffer::PixelUnpackBuffer));
		if ( d_PBOs.back().create() == false ) {
			qCritical() << "Could not create PBO";
		}
	}

	d_context->doneCurrent();

	d_stopped.store(false);

	for (auto & fbo : d_PBOs) {
		onReleasedBuffer(&fbo);
	}
}


void TrackingVideoPlayerTask::onReleasedBuffer(QOpenGLBuffer *pbo) {
	std::cerr << "Got buffer " << (void*)pbo << std::endl;
	d_waitingPBOs.erase(pbo);

	TrackingVideoFrame frame;
	frame.FrameID = d_capture.get(cv::CAP_PROP_POS_FRAMES);
	frame.PositionMS = d_capture.get(cv::CAP_PROP_POS_MSEC);

	if ( d_stopped.load() == true || d_capture.grab() == false ) {
		if ( d_waitingPBOs.empty() ) {
			emit finished();
		}
		return;
	}


	d_context->makeCurrent(d_surface);
	if ( !pbo->bind() ) {
		qCritical() << "Could not bind pbo";
		d_context->doneCurrent();
		return;
	}

	frame.Buffer = pbo;
	frame.Width = d_width;
	frame.Height = d_height;

	pbo->allocate(3*d_width*d_height);
	void * mapped = pbo->map(QOpenGLBuffer::ReadWrite);
	cv::Mat mat(d_height,d_width,CV_8UC3,mapped);
	d_capture.retrieve(mat);
	pbo->unmap();
	d_context->doneCurrent();

	d_waitingPBOs.insert(pbo);
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
