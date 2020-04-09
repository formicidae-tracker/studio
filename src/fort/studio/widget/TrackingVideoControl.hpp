#pragma once

#include <QWidget>

#include "TrackingVideoPlayer.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

namespace Ui {
class TrackingVideoControl;
}

class IdentifierBridge;

class TrackingVideoControl : public QWidget {
	Q_OBJECT
public:
	explicit TrackingVideoControl(QWidget *parent = 0);
	~TrackingVideoControl();

	void setup(TrackingVideoPlayer * player,
	           IdentifierBridge * identifier);

signals:
	void seek(fm::Duration duration);
	void play();
	void pause();
	void stop();
	void next();
	void prev();

	void playbackSpeedChanged(qreal value);

	void zoomFocusChanged(quint32 antID,qreal zoom);

private slots:
	void onPlayerPlaybackStateChanged(TrackingVideoPlayer::State state);
	void onPlayerPlaybackRateChanged(qreal rate);

	void onPlayerPositionChanged(fm::Duration positionMS);
	void onPlayerDurationChanged(const fm::Time & time, fm::Duration durationMS);

	void onAntSelection(bool selected);

	void on_stopButton_clicked();
	void on_playButton_clicked();
	void on_comboBox_currentIndexChanged(int);

	void on_zoomSlider_valueChanged(int value);
	void on_zoomCheckBox_stateChanged(int value);

private:
	qreal zoomValue() const;
	quint32 currentAntID() const;
	static QString formatDuration(fm::Duration duration);

	Ui::TrackingVideoControl * d_ui;
	TrackingVideoPlayer    * d_player;
	IdentifierBridge       * d_identifier;
};
