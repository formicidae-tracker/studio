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
	, d_controller(NULL){
    d_ui->setupUi(this);
    static MainWindow * myself = this;

    onNewController(NULL);
    loadSettings();
    connect(this,SIGNAL(newController(ExperimentController *)),
            d_ui->antList,SLOT(onNewController(ExperimentController *)));
    connect(this,SIGNAL(newController(ExperimentController *)),
            d_ui->experimentEditor,SLOT(onNewController(ExperimentController *)));
    connect(this,SIGNAL(newController(ExperimentController *)),
            d_ui->taggingWidget,SLOT(onNewController(ExperimentController *)));


    connect(this,SIGNAL(newController(ExperimentController *)),
            this,SLOT(onNewController(ExperimentController *)));

    connect(d_ui->antList,SIGNAL(antSelected(fort::myrmidon::Ant::ID)),
            d_ui->taggingWidget,SLOT(onAntSelected(fort::myrmidon::Ant::ID)));
}

MainWindow::~MainWindow() {
    delete d_ui;
}



void MainWindow::on_actionNew_triggered() {
	Error err = maybeSave();
	if ( err == UserDiscard ) {
		return;
	}
	if (err.OK() == false) {
		qCritical() << err.what();
		return;
	}

	QString path = promptPath();
	if ( path.isEmpty() ) {
		return;
	}
	ExperimentController * newCont = ExperimentController::create(path,this,err);
	if ( err.OK() == false ) {
		qCritical() << err.what();\
		return;
	}
	emit newController(newCont);
	qInfo() << tr("Created new file '%1'").arg(path);

}

void MainWindow::on_actionOpen_triggered() {
	Error err = maybeSave();
	if ( err == UserDiscard ) {
		return;
	}
	if (err.OK() == false) {
		qCritical() << err.what();
		return;
	}

	QString dir = "";
	if ( d_controller != NULL && !d_controller->experiment().AbsoluteFilePath().empty()) {
		dir = d_controller->experiment().Basedir().c_str();
	}

	QString filename = QFileDialog::getOpenFileName(this,"Open an experiment",
	                                                dir,
	                                                tr("FORT Experiment (*.myrmidon)"));

	if (filename.isEmpty() ) {
		return;
	}

	ExperimentController * newCont = ExperimentController::open(filename,this,err);
	if ( err.OK() == false ) {
		qCritical() << err.what();
		return;
	}
	emit newController(newCont);
	qInfo() << tr("Opened '%1'").arg(filename);
}


void MainWindow::on_actionQuit_triggered() {
	this->close();
}

void MainWindow::on_actionSave_triggered() {
	if ( d_controller == NULL ) {
		return;
	}
	Error err = saveAll(d_controller->experiment().AbsoluteFilePath().c_str());
	if ( err.OK() == false ) {
		qCritical() << err.what();
		return;
	}
	qInfo() << tr("Saved '%1'").arg(d_controller->experiment().AbsoluteFilePath().c_str());
}

void MainWindow::on_actionSaveAs_triggered() {
	if ( d_controller == NULL) {
		return;
	}
	QString path = promptPath();
	if ( path.isEmpty() ) {
		return;
	}

	Error err = saveAll(path);
    if (!err.OK()) {
	    qCritical() << err.what();
	    return;
    }
    pushRecent();
}


QString MainWindow::promptPath() {
	QFileDialog dialog(this, tr("Save file"),"untilted.myrmidon");
	dialog.setNameFilter(tr("FORT Experiment (*.myrmidon)"));
	dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(".myrmidon");
    if ( d_controller != NULL ) {
	    dialog.setDirectory(d_controller->experiment().Basedir().c_str());
    }
    if (dialog.exec() != QDialog::Accepted) {
	    return  "";
    }
    return dialog.selectedFiles().first();
}

void MainWindow::on_actionAddTrackingDataDir_triggered() {

}


void MainWindow::onExperimentModified(bool modified) {
	if ( modified == true ) {
		d_ui->actionSave->setEnabled(true);
	} else {
		d_ui->actionSave->setEnabled(false);
	}
}

const Error MainWindow::UserDiscard("User Discarded Action");

Error MainWindow::maybeSave() {
	if (d_controller == NULL || d_controller->isModified() == false ) {
		return Error::NONE;
	}

	const QMessageBox::StandardButton res
		= QMessageBox::warning(this, tr("Application"),
                               tr("The experiment data has been modified.\n"
                                  "Do you want to save your changes?"),
		                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	switch(res) {
	case QMessageBox::Save:
		return saveAll(d_controller->experiment().AbsoluteFilePath().c_str());
	case QMessageBox::Cancel:
		return UserDiscard;
	default:
		break;
	}
	return Error::NONE;
}


void MainWindow::closeEvent(QCloseEvent *e) {
	QSettings settings;
	settings.setValue("window/geometry",saveGeometry());
	settings.setValue("window/state",saveState());


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
	if ( d_controller == NULL ) {
		return;
	}
	QString newPath = d_controller->experiment().AbsoluteFilePath().c_str();

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
	restoreGeometry(settings.value("window/geometry").toByteArray());
	restoreState(settings.value("window/state").toByteArray());
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
			qCritical() << err.what(); \
			return; \
		} \
		if ( i > d_recentPaths.size() ) { \
			return; \
		} \
		ExperimentController * newCont = ExperimentController::open(d_recentPaths[i-1],this,err); \
		if ( err.OK() == false ) { \
			qCritical() << err.what(); \
			return; \
		} \
		emit newController(newCont); \
		qInfo() << tr("Opened '%1'").arg(d_recentPaths[i-1]); \
	}

IMPLEMENT_RECENT_FILE_SLOT(1);
IMPLEMENT_RECENT_FILE_SLOT(2);
IMPLEMENT_RECENT_FILE_SLOT(3);
IMPLEMENT_RECENT_FILE_SLOT(4);
IMPLEMENT_RECENT_FILE_SLOT(5);


void MainWindow::onNewController(ExperimentController * controller) {
	if ( d_controller != NULL ) {
		disconnect(d_controller,
		           SIGNAL(modified(bool)),
		           this,
		           SLOT(onExperimentModified(bool)));
		delete d_controller;
	}
	d_controller = controller;
	if ( d_controller == NULL ) {
		d_ui->actionSave->setEnabled(false);
		d_ui->actionSaveAs->setEnabled(false);
		d_ui->actionAddTrackingDataDir->setEnabled(false);
		return;
	}
	d_ui->actionSave->setEnabled(d_controller->isModified());
	d_ui->actionSaveAs->setEnabled(true);
	d_ui->actionAddTrackingDataDir->setEnabled(true);
	connect(d_controller,
	        SIGNAL(modified(bool)),
	        this,
	        SLOT(onExperimentModified(bool)));
	pushRecent();
}

void MainWindow::on_antList_antSelected(fort::myrmidon::Ant::ID i) {
	qInfo() << "Ant " << i <<  " selected";
}



Error MainWindow::saveAll(const QString & path) {
	if ( d_controller == NULL ) {
		return Error("No experiment loaded: nothing to save");
	}

	Error error = d_controller->save(path);
	if ( error.OK() == false ) {
		return error;
	}

	return d_ui->taggingWidget->save();
}
