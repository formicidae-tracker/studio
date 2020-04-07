#include "VideoPlayerControl.hpp"
#include "ui_VideoPlayerControl.h"

#include <fort/studio/Format.hpp>

VideoPlayerControl::VideoPlayerControl(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VideoPlayerControl)
	, d_player(nullptr) {
	d_ui->setupUi(this);
}

VideoPlayerControl::~VideoPlayerControl(){
	delete d_ui;
}


void VideoPlayerControl::setup(TrackingVideoPlayer * player) {
	d_player = player;
	connect(player,
	        &TrackingVideoPlayer::playbackStateChanged,
	        this,
	        &VideoPlayerControl::onPlayerPlaybackStateChanged);
	onPlayerPlaybackStateChanged(d_player->playbackState());
	connect(player,
	        &TrackingVideoPlayer::durationChanged,
	        this,
	        &VideoPlayerControl::onPlayerDurationChanged);
	connect(player,
	        &TrackingVideoPlayer::positionChanged,
	        this,
	        &VideoPlayerControl::onPlayerPositionChanged);

}


void VideoPlayerControl::onPlayerPlaybackStateChanged(TrackingVideoPlayer::State state) {
	switch(state) {
	case TrackingVideoPlayer::State::Playing:
		d_ui->stopButton->setEnabled(true);
		d_ui->playButton->setIcon(QIcon::fromTheme("media-playback-pause"));
		break;
	case TrackingVideoPlayer::State::Paused:
		d_ui->stopButton->setEnabled(true);
		d_ui->playButton->setIcon(QIcon::fromTheme("media-playback-start"));
		break;
	case TrackingVideoPlayer::State::Stopped:
		d_ui->stopButton->setEnabled(false);
		d_ui->playButton->setIcon(QIcon::fromTheme("media-playback-start"));
		break;
	}

	connect(d_ui->horizontalSlider,
	        &QAbstractSlider::sliderMoved,
	        d_player,
	        [this]( int value ) {
		        d_player->setPosition(qint64(value) * fm::Duration::Millisecond);
	        },
	        Qt::QueuedConnection);
}

void VideoPlayerControl::onPlayerPositionChanged(fm::Duration position) {
	auto currentTime = d_player->start().Add(position);
	d_ui->currentLabel->setText(ToQString(currentTime));
	d_ui->remainingLabel->setText(formatDuration(position- d_player->duration()));
	if ( d_ui->horizontalSlider->isSliderDown() == false ) {
		d_ui->horizontalSlider->setValue(position.Milliseconds());
	}
}

void VideoPlayerControl::onPlayerDurationChanged(const fm::Time & time, fm::Duration duration) {
	d_ui->horizontalSlider->setMinimum(0);
	d_ui->horizontalSlider->setMaximum(duration.Milliseconds());
	if ( d_player != nullptr ) {
		onPlayerPositionChanged(d_player->position());
	}
}


void VideoPlayerControl::on_stopButton_clicked() {
	if ( d_player == nullptr ) {
		return;
	}
	d_player->stop();
}

void VideoPlayerControl::on_playButton_clicked() {
	if ( d_player == nullptr ) {
		return;
	}
	switch( d_player->playbackState() ) {
	case TrackingVideoPlayer::State::Playing:
		d_player->pause();
		break;
	case TrackingVideoPlayer::State::Paused:
	case TrackingVideoPlayer::State::Stopped:
		d_player->play();
		break;
	}
}


QString VideoPlayerControl::formatDuration(fm::Duration duration) {
	QString format = "%1:%2:%3.%4";
	if ( duration < 0 ) {
		duration = -duration;
		format = "-" + format;
	}
	int hours(0),minutes(0),seconds(0);

	if ( duration > fm::Duration::Hour ) {
		hours = std::floor(duration.Hours());
		duration = duration - hours * fm::Duration::Hour;
	}

	if ( duration > fm::Duration::Minute ) {
		minutes = std::floor(duration.Minutes());
		duration = duration - minutes * fm::Duration::Minute;
	}

	while ( duration > fm::Duration::Second ) {
		seconds = std::floor(duration.Seconds());
		duration = duration - seconds * fm::Duration::Second;
	}

	return format
		.arg(hours,2,10,QLatin1Char('0'))
		.arg(minutes,2,10,QLatin1Char('0'))
		.arg(seconds,2,10,QLatin1Char('0'))
		.arg(int(duration.Milliseconds()),3,10,QLatin1Char('0'));
}
