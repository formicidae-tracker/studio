#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void on_actionQuit_triggered();
	void on_actionSave_triggered();
	void on_actionAddTrackingDataDir_triggered();
	void on_actionSaveAs_triggered();


private:
    Ui::MainWindow *d_ui;
};
