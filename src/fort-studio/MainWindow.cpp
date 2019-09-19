#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow)
	, d_experiment(this){
	d_experiment.setObjectName("experiment");
    d_ui->setupUi(this);
    d_ui->actionSave->setEnabled(false);
    setCurrentFile("");
}

MainWindow::~MainWindow() {
    delete d_ui;
}

void MainWindow::promptError(const Error & err) {
	QMessageBox::warning(this, tr("Internal Error"),
	                     tr("Unhandled internal error:\n"
	                        "%1\n").arg(err.what()),
	                     QMessageBox::Cancel);
}


void MainWindow::on_actionNew_triggered() {
	Error err = maybeSave();
	if ( err == UserDiscard ) {
		return;
	}
	if (err.OK() == false) {
		promptError(err);
		return;
	}

	d_experiment.reset();
	d_ui->statusbar->showMessage(tr("New experimental data"),2000);
	setCurrentFile("");
}

void MainWindow::on_actionOpen_triggered() {
	Error err = maybeSave();
	if ( err == UserDiscard ) {
		return;
	}
	if (err.OK() == false) {
		promptError(err);
		return;
	}

	QString dir = "";
	if (!d_currentFile.isEmpty()) {
		dir = QFileInfo(d_currentFile).absolutePath();
	}

	QString filename = QFileDialog::getOpenFileName(this,"Open an experiment",
	                                                dir,
	                                                tr("FORT Studio Experiment (*.fortstudio)"));

	if (filename.isEmpty() ) {
		return;
	}

	err = d_experiment.open(filename);
	if (!err.OK()) {
		promptError(err);
		return;
	}
	setCurrentFile(filename);
	d_ui->statusbar->showMessage(tr("Opened '%1'").arg(filename),2000);
}


void MainWindow::on_actionQuit_triggered() {
	this->close();
}

void MainWindow::on_actionSave_triggered() {
	Error err = save();
	if ( err.OK()) {
		return;
	}
	promptError(err);
}

void MainWindow::on_actionSaveAs_triggered() {
	Error err = saveAs();
    if (err.OK()) {
	    return;
    }
    promptError(err);
}

Error MainWindow::save() {
	if (d_currentFile.isEmpty() ) {
		return saveAs();
	} else {
		return save(d_currentFile);
	}
}

Error MainWindow::saveAs() {
	QFileDialog dialog(this, tr("Save file"),"untilted.fortstudio");
	dialog.setNameFilter(tr("FORT Studio Experiment (*.fortstudio)"));
	dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(".fortstudio");
    if (dialog.exec() != QDialog::Accepted) {
	    return  Error::NONE;
    }
    return save(dialog.selectedFiles().first());
}

void MainWindow::on_actionAddTrackingDataDir_triggered() {
	qInfo() << "Add tracking data trigger";
	QString dataDir  = QFileDialog::getExistingDirectory(this, tr("Open Tracking Data Directory"),
	                                                     "",
	                                                     QFileDialog::ShowDirsOnly);
	d_experiment.addDataDirectory(dataDir);
}


void MainWindow::on_experiment_modified() {
	d_ui->actionSave->setEnabled(true);
}


const Error MainWindow::UserDiscard("User Discarded Action");

Error MainWindow::maybeSave() {
	if (d_experiment.isModified() == false ) {
		return Error::NONE;
	}

	const QMessageBox::StandardButton res
		= QMessageBox::warning(this, tr("Application"),
                               tr("The experiment data has been modified.\n"
                                  "Do you want to save your changes?"),
		                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	switch(res) {
	case QMessageBox::Save:
		return save();
	case QMessageBox::Cancel:
		return UserDiscard;
	default:
		break;
	}
	return Error::NONE;
}


Error MainWindow::save(const QString & path) {
	Error err = d_experiment.save(path);
	if (!err.OK()) {
		d_ui->statusbar->showMessage(tr("Could not save file '%1': %2").arg(path).arg(err.what()),2000);
		return err;
	}
	setCurrentFile(path);
	d_ui->statusbar->showMessage(tr("File '%1' saved").arg(d_currentFile), 2000);
	return err;
}

void MainWindow::setCurrentFile(const QString & path) {
	d_currentFile = path;
	setWindowModified(false);
	QString shownName = d_currentFile;
	if (d_currentFile.isEmpty()) {
		shownName = "untitled.fortstudio";
	}
	setWindowFilePath(shownName);
	setWindowTitle(shownName);
}


void MainWindow::closeEvent(QCloseEvent *e) {
	Error err = maybeSave();

	if ( err.OK() ) {
		e->accept();
		return;
	} else if ( err == UserDiscard ) {
		e->ignore();
		return;
	}

	const QMessageBox::StandardButton res
		= QMessageBox::warning(this, tr("Application"),
                               tr("Could not save file:\n"
                                  "%1\n"
                                  "Do you want to quit and discard changes ?").arg(err.what()),
		                       QMessageBox::Discard | QMessageBox::Cancel);

	switch(res) {
	case QMessageBox::Discard:
		e->accept();
		break;
	case QMessageBox::Cancel:
	default:
		e->ignore();
	}
}
