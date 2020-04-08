#pragma once


#include <QWidget>


namespace Ui {
class VisualizationWidget;
}

class ExperimentBridge;
class TrackingVideoPlayer;

class VisualizationWidget : public QWidget {
	Q_OBJECT
public:
	explicit VisualizationWidget(QWidget *parent = 0);
	~VisualizationWidget();

	void setup(ExperimentBridge * experiment);

private:
	Ui::VisualizationWidget * d_ui;
	TrackingVideoPlayer     * d_videoPlayer;
};
