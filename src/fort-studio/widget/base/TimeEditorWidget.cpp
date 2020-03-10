#include "TimeEditorWidget.hpp"
#include "ui_TimeEditorWidget.h"

TimeEditorWidget::TimeEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TimeEditorWidget) {
	d_ui->setupUi(this);
}

TimeEditorWidget::~TimeEditorWidget() {
	delete d_ui;
}
