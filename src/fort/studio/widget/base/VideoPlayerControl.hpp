#pragma once

#include <QWidget>

#include <fort/studio/widget/TrackingVideoPlayer.hpp>

#include <fort/studio/MyrmidonTypes.hpp>

namespace Ui {
class VideoPlayerControl;
}

class VideoPlayerControl : public QWidget {
	Q_OBJECT
public:
	explicit VideoPlayerControl(QWidget *parent = 0);
	~VideoPlayerControl();

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
	void onPlayerPositionChanged(fm::Duration positionMS);
	void onPlayerDurationChanged(const fm::Time & time, fm::Duration durationMS);

	void on_stopButton_clicked();
	void on_playButton_clicked();

private:
	static QString formatDuration(fm::Duration duration);

	Ui::VideoPlayerControl * d_ui;
	TrackingVideoPlayer    * d_player;
};
