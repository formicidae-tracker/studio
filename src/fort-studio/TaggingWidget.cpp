#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"

TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget) {
    d_ui->setupUi(this);
}

TaggingWidget::~TaggingWidget() {
    delete d_ui;
}
