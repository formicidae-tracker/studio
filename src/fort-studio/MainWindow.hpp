#pragma once

#include <QMainWindow>

#include "Experiment.hpp"

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

	void on_experiment_modified();
	void on_experiment_pathModified(const QString & path);
protected:
    void closeEvent(QCloseEvent *event) override;

private:

	Error maybeSave();
	Error save();
	Error saveAs();
	Error save(const QString & path);

	void open(const QString & path);

	void setCurrentFile(const QString & path);
	void promptError(const Error & e);
	void pushRecent();
	void loadSettings();
	void rebuildRecentsFiles();

	const static Error UserDiscard;


    Ui::MainWindow *d_ui;


	bool    d_modified;
	Experiment d_experiment;
	std::deque<QString> d_recentPaths;
};
