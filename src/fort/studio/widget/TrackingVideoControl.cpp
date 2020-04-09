#include "TrackingVideoControl.hpp"
#include "ui_TrackingVideoControl.h"

#include <fort/studio/Format.hpp>

TrackingVideoControl::TrackingVideoControl(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TrackingVideoControl)
	, d_player(nullptr) {
	d_ui->setupUi(this);

	d_ui->comboBox->addItem("x 1.00",1.0);
	d_ui->comboBox->addItem("x 1.50",1.5);
	d_ui->comboBox->addItem("x 2.00",2.0);
	d_ui->comboBox->addItem("x 4.00",4.0);
	d_ui->comboBox->addItem("x 8.00",8.0);
}

TrackingVideoControl::~TrackingVideoControl(){
	delete d_ui;
}


void TrackingVideoControl::setup(TrackingVideoPlayer * player) {
	d_player = player;
	connect(player,
	        &TrackingVideoPlayer::playbackStateChanged,
	        this,
	        &TrackingVideoControl::onPlayerPlaybackStateChanged);
	onPlayerPlaybackStateChanged(d_player->playbackState());
	connect(player,
	        &TrackingVideoPlayer::durationChanged,
	        this,
	        &TrackingVideoControl::onPlayerDurationChanged);
	connect(player,
	        &TrackingVideoPlayer::positionChanged,
	        this,
	        &TrackingVideoControl::onPlayerPositionChanged);

	connect(d_ui->positionSlider,
	        &QAbstractSlider::sliderMoved,
	        d_player,
	        [this]( int value ) {
		        d_player->setPosition(qint64(value) * fm::Duration::Millisecond);
	        },
	        Qt::QueuedConnection);
	connect(d_player,
	        &TrackingVideoPlayer::seekReady,
	        d_ui->positionSlider,
	        &QWidget::setEnabled);
	d_ui->positionSlider->setEnabled(d_player->isSeekReady());

	onPlayerPlaybackRateChanged(d_player->playbackRate());
}


void TrackingVideoControl::onPlayerPlaybackStateChanged(TrackingVideoPlayer::State state) {
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
}

void TrackingVideoControl::onPlayerPositionChanged(fm::Duration position) {
	auto currentTime = d_player->start().Add(position);
	d_ui->currentLabel->setText(ToQString(currentTime));
	d_ui->remainingLabel->setText(formatDuration(position- d_player->duration()));
	if ( d_ui->positionSlider->isSliderDown() == false ) {
		d_ui->positionSlider->setValue(position.Milliseconds());
	}
}

void TrackingVideoControl::onPlayerDurationChanged(const fm::Time & time, fm::Duration duration) {
	d_ui->positionSlider->setMinimum(0);
	d_ui->positionSlider->setMaximum(duration.Milliseconds());
	if ( d_player != nullptr ) {
		onPlayerPositionChanged(d_player->position());
	}
}


void TrackingVideoControl::on_stopButton_clicked() {
	if ( d_player == nullptr ) {
		return;
	}
	d_player->stop();
}

void TrackingVideoControl::on_playButton_clicked() {
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


QString TrackingVideoControl::formatDuration(fm::Duration duration) {
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


void TrackingVideoControl::on_comboBox_currentIndexChanged(int index) {
	if ( index < 0 || d_player == nullptr ) {
		return;
	}
	d_player->setPlaybackRate(d_ui->comboBox->currentData().toDouble());
}

void TrackingVideoControl::onPlayerPlaybackRateChanged(qreal rate) {
	for( size_t i = 0 ; i < d_ui->comboBox->count(); ++i ) {
		if ( d_ui->comboBox->itemData(i).toDouble() == rate ) {
			d_ui->comboBox->setCurrentIndex(i);
			return;
		}
	}
	d_ui->comboBox->setCurrentIndex(-1);
}
