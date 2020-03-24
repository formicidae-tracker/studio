#include "UserDataWidget.hpp"
#include "ui_UserDataWidget.h"

UserDataWidget::UserDataWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UserDataWidget) {
	d_ui->setupUi(this);
}

UserDataWidget::~UserDataWidget() {
	delete d_ui;
}
