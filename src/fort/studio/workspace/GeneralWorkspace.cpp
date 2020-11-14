#include "GeneralWorkspace.hpp"
#include "ui_GeneralWorkspace.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>


GeneralWorkspace::GeneralWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_ui(new Ui::GeneralWorkspace) {
	d_ui->setupUi(this);
}

GeneralWorkspace::~GeneralWorkspace() {
	delete d_ui;
}


void GeneralWorkspace::initialize(ExperimentBridge * experiment) {
	d_ui->globalProperties->setup(experiment);
	d_ui->universeEditor->setup(experiment->universe());
}

void GeneralWorkspace::setUp(QMainWindow * main,const NavigationAction & actions) {
}

void GeneralWorkspace::tearDown(QMainWindow * maina,const NavigationAction & actions) {
}
