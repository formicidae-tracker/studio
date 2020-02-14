#pragma once

#include <QMainWindow>

#include <deque>

class ExperimentController;

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
	void on_actionSaveAs_triggered();
	void on_recentFile1_triggered();
	void on_recentFile2_triggered();
	void on_recentFile3_triggered();
	void on_recentFile4_triggered();
	void on_recentFile5_triggered();


	void on_controller_modified(bool);
	void on_controller_activated(bool);

protected:
    void closeEvent(QCloseEvent *event) override;

private:

	bool maybeSave(bool * cancelled = NULL);
	bool save(const QString & path);

	QString promptPath();


	void pushRecent();
	void loadSettings();
	void rebuildRecentsFiles();


    Ui::MainWindow       * d_ui;
	ExperimentController * d_controller;
	std::deque<QString>    d_recentPaths;


};
