#pragma once


#include "Workspace.hpp"

class QAction;

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
	void initialize(ExperimentBridge * experiment) override;
	void setUp(QMainWindow * main,const NavigationAction & actions) override;
	void tearDown(QMainWindow * main,const NavigationAction & actions) override;


private slots :
	void onCopyTimeActionTriggered();

	void jumpToTime();
private:
	ExperimentBridge           * d_experiment;
	Ui::VisualizationWorkspace * d_ui;
	TrackingVideoPlayer        * d_videoPlayer;
	QAction                    * d_jumpToTimeAction;
};
