#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QAbstractItemModel>
#include <QPointer>
#include <QSortFilterProxyModel>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include "Logger.hpp"

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>

#include <QToolBar>
#include <QPushButton>

#include <fort/studio/widget/vectorgraphics/Polygon.hpp>

QPointer<Logger> myLogger;


VisibilityActionController::VisibilityActionController(QWidget * widget,
                                                       QAction * action,
                                                       QObject * parent)
	: QObject(parent)
	, d_widget(widget)
	, d_action(action) {
	if ( action == nullptr || widget == nullptr) {
		return;
	}
	d_action->setCheckable(true);
	d_action->setChecked(d_widget->isVisible());

	d_widget->installEventFilter(this);

	connect(d_action,
	        &QAction::toggled,
	        this,
	        &VisibilityActionController::onActionToggled);
}

VisibilityActionController::~VisibilityActionController() {
	if ( d_action == nullptr || d_widget == nullptr ) {
		return;
	}
	// there are no clear order of deletion when child object are
	// destroyed, so d_widget could be invalid. so no remobing it to
	// avoid spurious segfault on exit.
	// d_widget->removeEventFilter(this);
}

void VisibilityActionController::onActionToggled(bool checked) {
	if ( d_widget == nullptr ) {
		return;
	}

	if ( checked == true && d_widget->isVisible() == false ) {
		d_widget->show();
	}
	if ( checked == false && d_widget->isVisible() == true ) {
		d_widget->close();
	}
}


bool VisibilityActionController::eventFilter(QObject * object, QEvent * event) {
	auto widget = d_widget;
	d_widget = nullptr;
	if ( event->type() == QEvent::Close ) {
		d_action->setChecked(false);
	}
	if ( event->type() == QEvent::Show ) {
		d_action->setChecked(true);
	}
	d_widget = widget;
	return false;
}


static void myLog(QtMsgType type, const QMessageLogContext &, const QString & msg) {
	myLogger->logMessage(type,msg);
}



MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow)
	, d_experiment(new ExperimentBridge(this))
	, d_logger( new Logger(this) )
	, d_loggerWidget(nullptr)
	, d_lastNavigatable(nullptr) {

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

	d_ui->globalProperties->setup(d_experiment,d_ui->actionLoadTagCloseUp);
	d_ui->universeEditor->setup(d_experiment->universe());
	d_ui->antList->setup(d_experiment->identifier());
	d_ui->taggingWidget->setup(d_experiment,d_ui->actionLoadTagCloseUp);
	d_ui->shappingWidget->setup(d_experiment);
	d_ui->zoningWidget->setup(d_experiment);
	d_ui->userData->setup(d_experiment->antMetadata());

	d_ui->shappingWidget->setEnabled(false);
	connect(d_ui->workspaceSelector,
	        &QTabWidget::currentChanged,
	        [this](int index) {
		        for ( size_t i = 0; i < d_ui->workspaceSelector->count(); ++i ) {
			        auto w = d_ui->workspaceSelector->widget(i);
			        w->setEnabled(i == index);
		        }
		        setupMoveActions();
	        });

	d_ui->workspaceSelector->setCurrentIndex(0);

	d_ui->visualizeWidget->setup(d_experiment);

	setWindowTitle(tr("FORmicidae Tracker Studio"));
	connect(d_experiment,
	        &ExperimentBridge::activated,
	        [this]() {
		        if (d_experiment->isActive() == false ) {
			        setWindowTitle(tr("FORmicidae Tracker Studio"));
		        }
		        setWindowTitle(tr("FORmicidae Tracker Studio - %1").arg(d_experiment->absoluteFilePath().c_str()));
	        });

	auto c = new VisibilityActionController(d_ui->dockWidget,d_ui->actionShowAntSelector,this);




	loadSettings();

	d_ui->menuEdit->addAction(d_ui->taggingWidget->newAntFromTagAction());
	d_ui->menuEdit->addAction(d_ui->taggingWidget->addIdentificationToAntAction());
	d_ui->menuEdit->addSeparator();
	d_ui->menuEdit->addAction(d_ui->taggingWidget->deletePoseEstimationAction());
	d_ui->menuEdit->addSeparator();
	d_ui->menuEdit->addAction(d_ui->shappingWidget->cloneAntShapeAction());

	d_ui->menuMove->addSeparator();
	d_ui->menuMove->addAction(d_ui->visualizeWidget->jumpToTimeAction());

	auto sorted = new QSortFilterProxyModel(this);
	sorted->setSourceModel(d_experiment->statistics()->stats());
	sorted->setSortRole(Qt::UserRole+1);
	d_ui->statsView->setModel(sorted);

	d_ui->statsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


	auto updateComputeStatsAction =
		[this] () {
			bool ready = d_experiment->statistics()->isReady();
			bool outdated = d_experiment->statistics()->isOutdated();
			d_ui->actionLoadTagStatistics->setEnabled(ready && outdated);
			d_ui->actionLoadTagStatistics->setText(ready ? tr("Compute Tag Statistics") : tr("Computing Tag Statistics..."));
		};

	updateComputeStatsAction();

	d_ui->computeStatsButton->setAction(d_ui->actionLoadTagStatistics);

	connect(d_experiment->statistics(),
	        &StatisticsBridge::ready,
	        this,updateComputeStatsAction);

	connect(d_experiment->statistics(),
	        &StatisticsBridge::outdated,
	        this,updateComputeStatsAction);

	connect(d_ui->actionLoadTagStatistics,
	        &QAction::triggered,
	        this,
	        [this]() {
		        d_experiment->statistics()->compute();
	        });


	auto updateLoadTagCloseUp =
		[this] () {
			bool ready = d_experiment->measurements()->isReady();
			bool outdated = d_experiment->measurements()->isOutdated();
			d_ui->actionLoadTagCloseUp->setEnabled(ready && outdated);
			d_ui->actionLoadTagCloseUp->setText(ready ? tr("Load Tag Close-Ups") : tr("Loading Tag Close-Ups ..."));
		};
	updateLoadTagCloseUp();

	connect(d_experiment->measurements(),
	        &MeasurementBridge::ready,
	        this,updateLoadTagCloseUp);

	connect(d_experiment->measurements(),
	        &MeasurementBridge::outdated,
	        this,updateLoadTagCloseUp);

	connect(d_ui->actionLoadTagCloseUp,
	        &QAction::triggered,
	        this,
	        [this]() {
		        d_experiment->measurements()->loadTagCloseUp();
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


void MainWindow::setupMoveActions() {
	auto jumpTimeAction = d_ui->visualizeWidget->jumpToTimeAction();
	jumpTimeAction->setEnabled(d_ui->workspaceSelector->currentWidget() == d_ui->visualizeWidget);

	NavigationAction actions {
	                          .NextTag = d_ui->actionNextTag,
	                          .PreviousTag = d_ui->actionPreviousTag,
	                          .NextCloseUp = d_ui->actionNextCloseUp,
	                          .PreviousCloseUp = d_ui->actionPreviousCloseUp,
	                          .CopyCurrentTime = d_ui->actionCopyTimeFromFrame,
	};


	if ( d_lastNavigatable != nullptr ) {
		d_lastNavigatable->tearDown(actions);
		d_ui->actionNextTag->setEnabled(false);
		d_ui->actionPreviousTag->setEnabled(false);
		d_ui->actionNextCloseUp->setEnabled(false);
		d_ui->actionPreviousCloseUp->setEnabled(false);
		d_ui->actionCopyTimeFromFrame->setEnabled(false);
		d_lastNavigatable = nullptr;
	}

	Navigatable* navigatable = dynamic_cast<Navigatable*>(d_ui->workspaceSelector->currentWidget());
	if ( navigatable == nullptr ) {
		return;
	}
	navigatable->setUp(actions);

	d_lastNavigatable = navigatable;
}
