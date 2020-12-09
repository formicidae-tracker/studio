#pragma once

#include "Workspace.hpp"

class QAction;

class QDockWidget;
class AntDisplayListWidget;

namespace Ui {
class VisualizationWorkspace;
}

class ExperimentBridge;
class TrackingVideoPlayer;

class VisualizationWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit VisualizationWorkspace(QWidget *parent = 0);
	virtual ~VisualizationWorkspace();


	QAction * jumpToTimeAction() const;

protected:
	void initialize(QMainWindow * main,ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;


private slots :
	void onCopyTimeActionTriggered();

	void jumpToTime();
private:
	ExperimentBridge           * d_experiment;
	Ui::VisualizationWorkspace * d_ui;
	TrackingVideoPlayer        * d_videoPlayer;
	QAction                    * d_jumpToTimeAction;
	AntDisplayListWidget       * d_antDisplay;
	QDockWidget                * d_antDisplayDock;
};
