#pragma once


#include <QWidget>


class QAction;

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

	QAction * copyCurrentTimeAction() const;

private slots:
	void onCopyTimeActionTriggered();

private:
	Ui::VisualizationWidget * d_ui;
	TrackingVideoPlayer     * d_videoPlayer;
	QAction                 * d_copyTimeAction;
};
