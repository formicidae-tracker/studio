#pragma once

#include <QMainWindow>

#include <deque>

#include "Navigatable.hpp"

class ExperimentBridge;
class Logger;
class LoggerWidget;

namespace Ui {
class MainWindow;
}

class VisibilityActionController : public QObject {
	Q_OBJECT
public :
	VisibilityActionController(QWidget * widget,
	                           QAction * action,
	                           QObject * parent = nullptr);
	virtual ~VisibilityActionController();

private slots:
	void onActionToggled(bool checked);
protected:
	bool eventFilter(QObject * object, QEvent * event) override;
private:
	QWidget * d_widget;
	QAction * d_action;
};


class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void on_actionOpen_triggered();
	void on_actionNew_triggered();
	void on_actionQuit_triggered();
	void on_actionSave_triggered();
	void on_actionSaveAs_triggered();
	void on_recentFile1_triggered();
	void on_recentFile2_triggered();
	void on_recentFile3_triggered();
	void on_recentFile4_triggered();
	void on_recentFile5_triggered();

	void on_actionShowLog_triggered();

	void onExperimentModified(bool);
	void onExperimentActivated(bool);

	void onLoggerWidgetDestroyed();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
	void setupMoveActions();
	bool maybeSave(bool * cancelled = NULL);
	bool save(const QString & path);

	QString promptPath();


	void pushRecent();
	void loadSettings();
	void rebuildRecentsFiles();


    Ui::MainWindow      * d_ui;
	ExperimentBridge    * d_experiment;
	std::deque<QString>   d_recentPaths;
	Logger              * d_logger;
	LoggerWidget        * d_loggerWidget;
	QtMessageHandler      d_handler;
	Navigatable         * d_lastNavigatable;
};
