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
#include <QTabBar>
#include <QComboBox>
#include <QLabel>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/widget/Logger.hpp>


#include <QToolBar>
#include <QPushButton>


QPointer<Logger> myLogger;

static void myLog(QtMsgType type, const QMessageLogContext &, const QString & msg) {
	myLogger->logMessage(type,msg);
}


void MainWindow::setUpLogger() {
	myLogger = d_logger;

	d_handler = qInstallMessageHandler(myLog);

	d_logStatus = new LogStatusWidget(d_logger,this);
	d_ui->statusBar->addPermanentWidget(d_logStatus);
	connect(d_logStatus,
	        &LogStatusWidget::showLog,
	        d_ui->actionShowLog,
	        &QAction::trigger);

}

void MainWindow::setUpSaveAndModificationEvents() {
	connect(d_experiment,
	        &ExperimentBridge::modified,
	        this,
	        &MainWindow::onExperimentModified);

	connect(d_experiment,
	        &ExperimentBridge::activated,
	        this,
	        &MainWindow::onExperimentActivated);
}

void MainWindow::setUpDynamicWindowTitle() {
	setWindowTitle(tr("FORmicidae Tracker Studio"));
	connect(d_experiment,
	        &ExperimentBridge::activated,
	        [this]() {
		        if (d_experiment->isActive() == false ) {
			        setWindowTitle(tr("FORmicidae Tracker Studio"));
		        }
		        setWindowTitle(tr("FORmicidae Tracker Studio - %1").arg(d_experiment->absoluteFilePath().c_str()));
	        });
}

void MainWindow::setUpWorkspaces() {
	std::vector<Workspace*> workspace
		= {
		   d_ui->generalWorkspace,
		   d_ui->tagStatisticsWorkspace,
		   d_ui->identificationWorkspace,
		   d_ui->antShapeWorkspace,
		   d_ui->antMeasurementWorkspace,
		   d_ui->zoningWorkspace,
		   d_ui->antMetadataWorkspace,
		   d_ui->visualizationWorkspace,
	};

	for ( const auto & w : workspace ) {
		w->initialize(this,d_experiment);
		w->setEnabled(false);
	}

	d_ui->workspaceSelector->setCurrentIndex(0);

	connect(d_ui->workspaceSelector,
	        &QTabWidget::currentChanged,
	        this,
	        &MainWindow::onCurrentWorkspaceChanged);

	onCurrentWorkspaceChanged(0);
	d_ui->workspaceSelector->setElideMode(Qt::ElideRight);

}

void MainWindow::setUpWorkspacesActions() {
	d_ui->menuEdit->addAction(d_ui->identificationWorkspace->newAntFromTagAction());
	d_ui->menuEdit->addAction(d_ui->identificationWorkspace->addIdentificationToAntAction());
	d_ui->menuEdit->addSeparator();
	d_ui->menuEdit->addAction(d_ui->identificationWorkspace->deletePoseEstimationAction());
	d_ui->menuEdit->addSeparator();
	d_ui->menuEdit->addAction(d_ui->antShapeWorkspace->cloneAntShapeAction());
}

void MainWindow::setUpNavigationActions() {
	auto toolbar = new QToolBar(this);
	d_navigationActions.NavigationToolBar = toolbar;
	addToolBar(toolbar);


#define set_button(res,symbolStr,legendStr,shortCutStr,toolTipStr) do {	  \
		(res) = toolbar->addAction(QIcon::fromTheme(symbolStr), \
		                           tr(legendStr)); \
		(res)->setShortcut(QKeySequence(shortCutStr)); \
		(res)->setToolTip(tr(toolTipStr " (" shortCutStr ")")); \
		(res)->setStatusTip((res)->toolTip()); \
		(res)->setEnabled(false); \
	}while(0)
	set_button(d_navigationActions.PreviousTag,
	           "go-first-symbolic",
	           "Previous Tag",
	           "Alt+W",
	           "Jump to previous tag");

	set_button(d_navigationActions.PreviousCloseUp,
	           "go-previous-symbolic",
	           "Previous Close Up",
	           "Alt+A",
	           "Jump to previous close-up");

	set_button(d_navigationActions.NextCloseUp,
	           "go-next-symbolic",
	           "Next Close Up",
	           "Alt+D",
	           "Jump to next close-up");

	set_button(d_navigationActions.NextTag,
	           "go-last-symbolic",
	           "Next Tag",
	           "Alt+S",
	           "Jump to next tag");

	toolbar->addSeparator();
	set_button(d_navigationActions.CopyCurrentTime,
	           "edit-copy-symbolic",
	           "Copy Current Time",
	           "Ctrl+Shift+C",
	           "Copy time of currently displayed frame or close-up");

	toolbar->addSeparator();
	set_button(d_navigationActions.JumpToTime,
	           "go-jump-symbolic",
	           "Jump To Time",
	           "Ctrl+T",
	           "Jump current movie to time");
#undef set_button

	d_ui->menuMove->addAction(d_navigationActions.NextCloseUp);
	d_ui->menuMove->addAction(d_navigationActions.PreviousCloseUp);
	d_ui->menuMove->addSeparator();
	d_ui->menuMove->addAction(d_navigationActions.NextTag);
	d_ui->menuMove->addAction(d_navigationActions.PreviousTag);
	d_ui->menuMove->addSeparator();
	d_ui->menuMove->addAction(d_navigationActions.JumpToTime);

	d_ui->menuEdit->addSeparator();
	d_ui->menuEdit->addAction(d_navigationActions.CopyCurrentTime);
	d_ui->menuEdit->addSeparator();
}

void MainWindow::setUpWorkspacesSelectionActions() {
	auto selectionGroup = new QActionGroup(this);
	auto comboBox = new QComboBox(this);

	for ( int i = 0; i < d_ui->workspaceSelector->count(); ++i ) {
		auto action = new QAction(this);
		auto name = d_ui->workspaceSelector->tabText(i);
		action->setText(tr("%1 Workspace").arg(name));
		action->setShortcut(QKeySequence(tr("Alt+%1").arg(i+1)));
		action->setToolTip(d_ui->workspaceSelector->tabToolTip(i));
		action->setStatusTip(d_ui->workspaceSelector->tabToolTip(i));
		action->setCheckable(true);
		comboBox->addItem(name);
		selectionGroup->addAction(action);
		d_ui->menuWorkspace->addAction(action);

		connect(action,&QAction::toggled,
		        this,[i,this](bool checked) {
			             if ( checked == true) {
				             d_ui->workspaceSelector->setCurrentIndex(i);
			             }
		             });

		connect(d_ui->workspaceSelector,
		        &QTabWidget::currentChanged,
		        this,
		        [i,action,this](int index) {
			        if ( index == i) {
				        action->setChecked(true);
			        }
		        });

	}
	d_ui->workspaceSelector->tabBar()->setVisible(false);

	connect(comboBox,qOverload<int>(&QComboBox::currentIndexChanged),
	        d_ui->workspaceSelector,&QTabWidget::setCurrentIndex);

	connect(d_ui->workspaceSelector,&QTabWidget::currentChanged,
	comboBox,&QComboBox::setCurrentIndex);

	auto tb = new QToolBar(this);
	tb->addWidget(new QLabel(tr("Workspace:")));
	tb->addWidget(comboBox);
	addToolBar(tb);
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow)
	, d_experiment(new ExperimentBridge(this))
	, d_logger( new Logger(this) )
	, d_loggerWidget(nullptr)
	, d_lastWorkspace(nullptr) {

	d_ui->setupUi(this);

	setUpLogger();
	setUpSaveAndModificationEvents();
	setUpDynamicWindowTitle();

	loadSettings();

	setUpWorkspacesSelectionActions();
	setUpNavigationActions();

	setUpWorkspaces();
	setUpWorkspacesActions();



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

	if ( d_experiment->open(filename,this) == false ) {
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
		d_experiment->open(d_recentPaths[i-1],this); \
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


void MainWindow::onCurrentWorkspaceChanged(int index) {
	auto currentWidget = d_ui->workspaceSelector->currentWidget();
	auto currentWorkspace = dynamic_cast<Workspace*>(currentWidget);

	for ( int i = 0; i < d_ui->workspaceSelector->count(); ++i ) {
		d_ui->workspaceSelector->widget(i)->setEnabled(index == i);
	}

	if ( d_lastWorkspace != nullptr ) {
		d_lastWorkspace->tearDown(d_navigationActions);
	}
	currentWorkspace->setUp(d_navigationActions);
	d_lastWorkspace = currentWorkspace;
}
