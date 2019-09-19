#pragma once

#include <QMainWindow>

#include "Experiment.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void  on_actionQuit_triggered();
	void  on_actionSave_triggered();
	void  on_actionAddTrackingDataDir_triggered();
	void on_actionSaveAs_triggered();

	void on_experiment_modified();

protected:
    void closeEvent(QCloseEvent *event) override;

private:

	Error maybeSave();
	Error save();
	Error saveAs();
	Error save(const QString & path);
	void  setCurrentFile(const QString & path);
	void  promptError(const Error & e);


	const static Error UserDiscard;


    Ui::MainWindow *d_ui;


	bool    d_modified;
	QString d_currentFile;
	Experiment d_experiment;
};
