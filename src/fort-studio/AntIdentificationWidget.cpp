#include "AntIdentificationWidget.hpp"
#include "ui_AntIdentificationWidget.h"

AntIdentificationWidget::AntIdentificationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntIdentificationWidget) {
	d_ui->setupUi(this);
}

AntIdentificationWidget::~AntIdentificationWidget() {
	delete d_ui;
}
