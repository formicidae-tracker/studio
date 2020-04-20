#include "ZoningWidget.hpp"
#include "ui_ZoningWidget.h"

ZoningWidget::ZoningWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ZoningWidget) {
	d_ui->setupUi(this);
}

ZoningWidget::~ZoningWidget() {
	delete d_ui;
}
