#pragma once

#include <QMainWindow>

#include "ExperimentController.hpp"

#include <deque>

namespace Ui {
class MainWindow;
}

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
	void on_actionAddTrackingDataDir_triggered();
	void on_actionSaveAs_triggered();
	void on_recentFile1_triggered();
	void on_recentFile2_triggered();
	void on_recentFile3_triggered();
	void on_recentFile4_triggered();
	void on_recentFile5_triggered();

	void on_antList_antSelected(uint32_t);

	void onExperimentModified(bool);
	void onNewController(ExperimentController * controller);


signals:
	void newController(ExperimentController * controller);

protected:
    void closeEvent(QCloseEvent *event) override;

private:

	Error maybeSave();
	Error saveAll(const QString & path);
	QString promptPath();



	void pushRecent();
	void loadSettings();
	void rebuildRecentsFiles();

	const static Error UserDiscard;

    Ui::MainWindow *d_ui;

	ExperimentController * d_controller;
	std::deque<QString> d_recentPaths;
};
