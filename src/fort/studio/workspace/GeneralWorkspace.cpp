#include "GeneralWorkspace.hpp"
#include "ui_GeneralWorkspace.h"

#include <QDockWidget>
#include <QMainWindow>

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <fort/studio/widget/AntListWidget.hpp>


GeneralWorkspace::GeneralWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_ui(new Ui::GeneralWorkspace) {
	d_ui->setupUi(this);

	d_antList = new AntSimpleListWidget(this);

	d_antListDock = new QDockWidget(tr("Ants"),this);
	d_antListDock->setWidget(d_antList);
}

GeneralWorkspace::~GeneralWorkspace() {
	delete d_ui;
}


void GeneralWorkspace::initialize(QMainWindow * main,ExperimentBridge * experiment) {
	d_ui->globalProperties->setup(experiment);
	d_ui->universeEditor->setup(experiment->universe());

	d_antList->initialize(experiment);

	main->addDockWidget(Qt::LeftDockWidgetArea,d_antListDock);
	d_antListDock->hide();


}

void GeneralWorkspace::setUp(const NavigationAction & actions) {
	d_antListDock->show();
}

void GeneralWorkspace::tearDown(const NavigationAction & actions) {
	d_antListDock->hide();
}
