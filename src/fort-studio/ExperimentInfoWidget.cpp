#include "ExperimentInfoWidget.hpp"
#include "ui_ExperimentInfoWidget.h"

ExperimentInfoWidget::ExperimentInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentInfoWidget)
{
	ui->setupUi(this);
}

ExperimentInfoWidget::~ExperimentInfoWidget()
{
	delete ui;
}
