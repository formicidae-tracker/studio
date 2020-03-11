#include "ShappingWidget.hpp"
#include "ui_ShappingWidget.h"

#include <fort-studio/bridge/ExperimentBridge.hpp>

ShappingWidget::ShappingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ShappingWidget) {
	d_ui->setupUi(this);
}

ShappingWidget::~ShappingWidget() {
	delete d_ui;
}


void ShappingWidget::setup(ExperimentBridge * experiment) {
	d_ui->shapeTypeEditor->setup(experiment->antShapeTypes());
	d_ui->measurementTypeEditor->setup(experiment->measurements());


	d_ui->comboBox->setModel(experiment->antShapeTypes()->shapeModel());

}
