#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QtDebug>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow) {
    d_ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete d_ui;
}


void MainWindow::on_actionQuit_triggered() {
	this->close();
}

void MainWindow::on_actionSave_triggered() {
	qInfo() << "Save trigger";
}

void MainWindow::on_actionSaveAs_triggered() {
	qInfo() << "Save as trigger";
}

void MainWindow::on_actionAddTrackingDataDir_triggered() {
	qInfo() << "Add tracking data trigger";
}
