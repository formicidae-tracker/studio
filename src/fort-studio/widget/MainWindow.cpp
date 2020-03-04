#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QAbstractItemModel>
#include <QPointer>


#include <fort-studio/bridge/ExperimentBridge.hpp>
#include "Logger.hpp"

#include <fort-studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort-studio/widget/base/ColorComboBox.hpp>
#include <QToolBar>
#include <QPushButton>

#include <fort-studio/widget/vectorgraphics/Polygon.hpp>

QPointer<Logger> myLogger;

static void myLog(QtMsgType type, const QMessageLogContext &, const QString & msg) {
	myLogger->logMessage(type,msg);
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow)
	, d_experiment(new ExperimentBridge(this))
	, d_logger( new Logger(this) )
	, d_loggerWidget(NULL) {

	d_ui->setupUi(this);

	myLogger = d_logger;

	d_handler = qInstallMessageHandler(myLog);

	connect(d_experiment,
	        &ExperimentBridge::modified,
	        this,
	        &MainWindow::onExperimentModified);

	connect(d_experiment,
	        &ExperimentBridge::activated,
	        this,
	        &MainWindow::onExperimentActivated);

	d_ui->globalProperties->setup(d_experiment->globalProperties());
	d_ui->universeEditor->setup(d_experiment->universe());
	d_ui->measurementType->setup(d_experiment->measurements());
	d_ui->antList->setup(d_experiment->identifier());

    loadSettings();

    auto scene = new VectorialScene(this);
	d_ui->graphicsView->setScene(scene);
	d_ui->graphicsView->setRenderHint(QPainter::Antialiasing,true);
	d_ui->graphicsView->setMouseTracking(true);
	scene->setMode(VectorialScene::Mode::InsertPolygon);
	connect(d_ui->graphicsView,
	        &VectorialView::zoomed,
	        scene,
	        &VectorialScene::onZoomed);

	auto toolbar = addToolBar(tr("test"));
	auto colorBox = new ColorComboBox(NULL);
	toolbar->addWidget(colorBox);
	connect(colorBox,
	        &ColorComboBox::colorChanged,
	        scene,
	        &VectorialScene::setColor);
	colorBox->setCurrentIndex(2);

	auto buttons = new QButtonGroup(this);
#define myAddButton(mode) \
	auto mode ## Button = new QPushButton(tr(#mode)); \
	connect( mode ## Button, &QPushButton::clicked, \
	         [scene]() { scene->setMode(VectorialScene::Mode::mode); }); \
	buttons->addButton(mode ## Button); \
	mode ## Button->setCheckable(true); \
	toolbar->addWidget(mode ## Button);

	myAddButton(Edit);
	myAddButton(InsertVector);
	myAddButton(InsertCapsule);
	myAddButton(InsertCircle);
	myAddButton(InsertPolygon);



	connect(scene,
	        &VectorialScene::modeChanged,
	        buttons,
	        [=](VectorialScene::Mode mode) {
		        switch(mode) {
		        case VectorialScene::Mode::Edit:
			        EditButton->setChecked(true);
			        break;
		        case VectorialScene::Mode::InsertVector:
			        InsertVectorButton->setChecked(true);
			        break;
		        case VectorialScene::Mode::InsertCircle:
			        InsertCircleButton->setChecked(true);
			        break;
		        case VectorialScene::Mode::InsertCapsule:
			        InsertCapsuleButton->setChecked(true);
			        break;
		        case VectorialScene::Mode::InsertPolygon:
			        InsertPolygonButton->setChecked(true);
			        break;
		        }
	        });

	scene->setMode(VectorialScene::Mode::InsertVector);


	connect(scene,
	        &VectorialScene::polygonCreated,
	        [](const Polygon * polygon) {
		        connect(polygon,
		                &Shape::updated,
		                [polygon]() {
			                std::cerr << "Polygon " << polygon << " updated." << std::endl;
		                });
			        });

}

MainWindow::~MainWindow() {
	qInstallMessageHandler(d_handler);
    delete d_ui;
}

void MainWindow::on_actionNew_triggered() {
	if ( maybeSave() == false ) {
		return;
	}

	QString path = promptPath();
	if ( path.isEmpty() ) {
		return;
	}

	if ( d_experiment->create(path) == false ) {
		return;
	}

	pushRecent();
}

void MainWindow::on_actionOpen_triggered() {
	if ( maybeSave() == false ) {
		return;
	}

	QString dir = "";
	if ( d_experiment->absoluteFilePath().empty() == false ) {
		dir = d_experiment->absoluteFilePath().parent_path().c_str();
	}

	QString filename = QFileDialog::getOpenFileName(this,"Open an experiment",
	                                                dir,
	                                                tr("FORT Experiment (*.myrmidon)"));

	if (filename.isEmpty() ) {
		return;
	}

	if ( d_experiment->open(filename) == false ) {
		return;
	}

	pushRecent();
}


void MainWindow::on_actionQuit_triggered() {
	this->close();
}

void MainWindow::on_actionSave_triggered() {
	d_experiment->save();
}

void MainWindow::on_actionSaveAs_triggered() {
	QString path = promptPath();
	if ( path.isEmpty() ) {
		return;
	}

	if ( d_experiment->saveAs(path) ) {
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
    if ( d_experiment->absoluteFilePath().empty() == false ) {
	    dialog.setDirectory(d_experiment->absoluteFilePath().parent_path().c_str());
    }
    if (dialog.exec() != QDialog::Accepted) {
	    return  "";
    }
    return dialog.selectedFiles().first();
}



bool MainWindow::maybeSave(bool * cancelled) {
#define fstudio_set_cancelled(value) do { \
		if ( cancelled != NULL ) { *cancelled = value; } \
	} while(0)

	if ( d_experiment->isModified() == false ) {
		fstudio_set_cancelled(false);
		return true;
	}

	const QMessageBox::StandardButton res
		= QMessageBox::warning(this, tr("Application"),
                               tr("The experiment data has been modified.\n"
                                  "Do you want to save your changes?"),
		                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	switch(res) {
	case QMessageBox::Save:
		fstudio_set_cancelled(false);
		return d_experiment->save();
	case QMessageBox::Cancel:
		fstudio_set_cancelled(true);
		return false;
	default:
		break;
	}

	fstudio_set_cancelled(false);
	return true;
}


void MainWindow::closeEvent(QCloseEvent *e) {
	QSettings settings;
	settings.setValue("window/geometry",saveGeometry());
	settings.setValue("window/state",saveState());


	bool cancelled = false;

	if ( maybeSave(&cancelled) == true ) {
		if ( d_loggerWidget ) {
			d_loggerWidget->close();
		}
		e->accept();
		return;
	} else if ( cancelled == true ) {
		e->ignore();
		return;
	}

	const QMessageBox::StandardButton res
		= QMessageBox::warning(this, tr("Application"),
		                       tr("Could not save file (see logs)\n"
		                          "Do you want to quit and discard changes ?"),
		                       QMessageBox::Discard | QMessageBox::Cancel);

	switch(res) {
	case QMessageBox::Discard:
		if ( d_loggerWidget ) {
			d_loggerWidget->close();
		}
		e->accept();
		break;
	case QMessageBox::Cancel:
	default:
		e->ignore();
	}

}

void MainWindow::pushRecent() {
	QString newPath = d_experiment->absoluteFilePath().c_str();

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
		if ( maybeSave() == false  ) { \
			return; \
		} \
		if ( i > d_recentPaths.size() ) { \
			return; \
		} \
		d_experiment->open(d_recentPaths[i-1]); \
	}

IMPLEMENT_RECENT_FILE_SLOT(1);
IMPLEMENT_RECENT_FILE_SLOT(2);
IMPLEMENT_RECENT_FILE_SLOT(3);
IMPLEMENT_RECENT_FILE_SLOT(4);
IMPLEMENT_RECENT_FILE_SLOT(5);


void MainWindow::onExperimentModified(bool modified) {
	d_ui->actionSave->setEnabled(modified);
}

void MainWindow::onExperimentActivated(bool active) {
	if (active == false) {
		d_ui->actionSave->setEnabled(false);
		d_ui->actionSaveAs->setEnabled(false);
		return;
	}
	d_ui->actionSave->setEnabled(d_experiment->isModified());
	d_ui->actionSaveAs->setEnabled(true);
}

void MainWindow::on_actionShowLog_triggered() {
	if ( d_loggerWidget != NULL ) {
		d_loggerWidget->raise();
		d_loggerWidget->activateWindow();
		return;
	}

	d_loggerWidget = new LoggerWidget(d_logger,NULL);
	d_loggerWidget->setAttribute(Qt::WA_DeleteOnClose);
	connect(d_loggerWidget,
	        &QObject::destroyed,
	        this,
	        &MainWindow::onLoggerWidgetDestroyed);

	d_loggerWidget->show();
}

void MainWindow::onLoggerWidgetDestroyed() {
	d_loggerWidget = NULL;
}
