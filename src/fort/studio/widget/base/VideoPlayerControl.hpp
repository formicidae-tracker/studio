#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes.hpp>

namespace Ui {
class VideoPlayerControl;
}

class VideoPlayerControl : public QWidget {
	Q_OBJECT
public:
	explicit VideoPlayerControl(QWidget *parent = 0);
	~VideoPlayerControl();

public slots:
	// void setPosition(qint64 durationNS);
	// void setPlaybackSpeed(double value);
signals:
	void seek(qint64 durationMS);
	void play();
	void pause();
	void stop();
	void next();
	void prev();
	void playbackSpeedChanged(double);

private:
	Ui::VideoPlayerControl * d_ui;
};
