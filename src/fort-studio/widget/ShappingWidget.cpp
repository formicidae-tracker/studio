#include "ShappingWidget.hpp"
#include "ui_ShappingWidget.h"

ShappingWidget::ShappingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ShappingWidget) {
	d_ui->setupUi(this);
}

ShappingWidget::~ShappingWidget() {
	delete d_ui;
}
