#pragma once

#include <QWidget>

#include "TrackingVideoPlayer.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

namespace Ui {
class TrackingVideoControl;
}

class TrackingVideoControl : public QWidget {
	Q_OBJECT
public:
	explicit TrackingVideoControl(QWidget *parent = 0);
	~TrackingVideoControl();

	void setup(TrackingVideoPlayer * player);

signals:
	void seek(fm::Duration duration);
	void play();
	void pause();
	void stop();
	void next();
	void prev();

	void playbackSpeedChanged(qreal value);

private slots:
	void onPlayerPlaybackStateChanged(TrackingVideoPlayer::State state);
	void onPlayerPlaybackRateChanged(qreal rate);

	void onPlayerPositionChanged(fm::Duration positionMS);
	void onPlayerDurationChanged(const fm::Time & time, fm::Duration durationMS);

	void on_stopButton_clicked();
	void on_playButton_clicked();
	void on_comboBox_currentIndexChanged(int);

private:
	static QString formatDuration(fm::Duration duration);

	Ui::TrackingVideoControl * d_ui;
	TrackingVideoPlayer    * d_player;
};
