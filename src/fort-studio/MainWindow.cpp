#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow)
	, d_experiment(this){
	d_experiment.setObjectName("experiment");
    d_ui->setupUi(this);
    static MainWindow * myself = this;
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) -> void{
	                           QByteArray localMsg = msg.toLocal8Bit();
	                           switch (type) {
	                           case QtDebugMsg:
		                           myself->d_ui->console->appendHtml("<font color=\"black\">"+msg+"</font></br>");
		                           break;
	                           case QtInfoMsg:
		                           myself->d_ui->console->appendHtml("<font color=\"blue\">"+msg+"</font></br>");
		                           break;
	                           case QtWarningMsg:
		                           myself->d_ui->console->appendHtml("<font color=\"orange\"><b>"+msg+"</b></font></br>");
		                           break;
	                           case QtCriticalMsg:
		                           myself->d_ui->console->appendHtml("<font color=\"red\">"+msg+"</font></br>");
		                           break;
	                           case QtFatalMsg:
		                           myself->d_ui->console->appendHtml("<font color=\"red\"><b>"+msg+"</b></font></br>");
		                           break;
	                           }
                           });

    d_ui->actionSave->setEnabled(false);
    setCurrentFile("");
    loadSettings();
    d_ui->antList->setExperiment(&d_experiment);
    d_ui->experimentInfo->setExperiment(&d_experiment);
    d_ui->actionAddTrackingDataDir->setEnabled(false);
}

MainWindow::~MainWindow() {
    delete d_ui;
}

void MainWindow::promptError(const Error & err) {
	qWarning() << "Unhandled internal error: " << err.what();
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
	if (d_experiment.AbsolutePath() == "") {
		on_actionSaveAs_triggered();
		return;
	}


	d_experiment.reset();
	qInfo() << tr("New experimental data");
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
	if (!d_experiment.AbsolutePath().isEmpty()) {
		dir = QFileInfo(d_experiment.AbsolutePath()).absolutePath();
	}

	QString filename = QFileDialog::getOpenFileName(this,"Open an experiment",
	                                                dir,
	                                                tr("FORT Studio Experiment (*.fortstudio)"));

	if (filename.isEmpty() ) {
		return;
	}
	open(filename);
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
    if (!err.OK()) {
	    promptError(err);
	    return;
    }
    pushRecent();
}

Error MainWindow::save() {
	if (d_experiment.AbsolutePath().isEmpty() ) {
		return saveAs();
	} else {
		return save(d_experiment.AbsolutePath());
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
	d_ui->experimentInfo->on_addButton_triggered();
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
		return err;
	}
	setCurrentFile(path);
	qInfo() << tr("File '%1' saved").arg(d_experiment.AbsolutePath());
	return err;
}

void MainWindow::setCurrentFile(const QString & path) {
	setWindowModified(false);
	QString shownName = d_experiment.AbsolutePath();
	if (shownName.isEmpty()) {
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

void MainWindow::pushRecent() {
	QString newPath = QFileInfo(d_experiment.AbsolutePath()).absoluteFilePath();

	//if already in the vector, just move it to the top
	if (!d_recentPaths.empty() && d_recentPaths[0] == newPath ) {
		return;
	}

	auto fi =  std::find(d_recentPaths.begin(),d_recentPaths.end(),newPath);
	if (fi != d_recentPaths.end() ) {
		d_recentPaths.erase(fi);
	}

	d_recentPaths.push_front(newPath);

	if ( d_recentPaths.size() > 5 ) {
		d_recentPaths.resize(5);
	}
	QSettings settings;
	for ( size_t i = 0; i < 5; ++i ) {
		QString data;
		if ( i < d_recentPaths.size() ) {
			data = d_recentPaths[i];
		}
		settings.setValue("recent-files/"+QString::number(i),data);
	}

	rebuildRecentsFiles();
}


void MainWindow::loadSettings() {
	d_recentPaths.clear();
	QSettings settings;
	for (size_t i = 0; i < 5; ++i ) {
		QString data = settings.value("recent-files/" + QString::number(i)).toString();
		if (data.isEmpty()) {
			continue;
		}
		d_recentPaths.push_back(data);
	}
	rebuildRecentsFiles();
}

void MainWindow::rebuildRecentsFiles() {
	std::vector<QAction*> actions = {d_ui->recentFile1,d_ui->recentFile2,d_ui->recentFile3,d_ui->recentFile4,d_ui->recentFile5};
	for ( size_t i = 0 ; i < 5 ; ++i ) {
		if ( i >= d_recentPaths.size() ) {
			actions[i]->setVisible(false);
			continue;
		}
		actions[i]->setText(d_recentPaths[i]);
		actions[i]->setVisible(true);
		actions[i]->setEnabled(QFileInfo::exists(d_recentPaths[i]));
	}
}


#define IMPLEMENT_RECENT_FILE_SLOT(i) \
	void MainWindow::on_recentFile ## i ## _triggered() { \
		Error err = maybeSave(); \
		if ( err == UserDiscard ) { \
			return; \
		} \
		if (err.OK() == false) { \
			promptError(err); \
			return; \
		} \
		if ( i > d_recentPaths.size() ) { \
			return; \
		} \
		open(d_recentPaths[i-1]); \
	}

IMPLEMENT_RECENT_FILE_SLOT(1);
IMPLEMENT_RECENT_FILE_SLOT(2);
IMPLEMENT_RECENT_FILE_SLOT(3);
IMPLEMENT_RECENT_FILE_SLOT(4);
IMPLEMENT_RECENT_FILE_SLOT(5);


void MainWindow::open(const QString & path ) {
	Error err = d_experiment.open(path);
	if (!err.OK()) {
		promptError(err);
		return;
	}
	setCurrentFile(path);
	qInfo() << tr("Opened '%1'").arg(path);
	pushRecent();
}


void MainWindow::on_experiment_pathModified(const QString & path) {
	d_ui->actionAddTrackingDataDir->setEnabled( d_experiment.AbsolutePath().isEmpty() == false);
}
