#pragma once


#include <QWidget>


namespace Ui {
class VisualizationWidget;
}

class MovieBridge;
class TrackingVideoPlayer;

class VisualizationWidget : public QWidget {
	Q_OBJECT
public:
	explicit VisualizationWidget(QWidget *parent = 0);
	~VisualizationWidget();

	void setup(MovieBridge * bridge);

private:
	Ui::VisualizationWidget * d_ui;
	MovieBridge             * d_bridge;
	TrackingVideoPlayer     * d_videoPlayer;
};
