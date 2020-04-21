#include "ZoningWidget.hpp"
#include "ui_ZoningWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

ZoningWidget::ZoningWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ZoningWidget) {
	d_ui->setupUi(this);
}

ZoningWidget::~ZoningWidget() {
	delete d_ui;
}

void ZoningWidget::setup(ExperimentBridge * experiment) {
	d_ui->zonesEditor->setup(experiment->zones());
}
