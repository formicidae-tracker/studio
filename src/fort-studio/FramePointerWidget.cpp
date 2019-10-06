#include "FramePointerWidget.hpp"
#include "ui_FramePointerWidget.h"

FramePointerWidget::FramePointerWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::FramePointerWidget) {
	d_ui->setupUi(this);
}

FramePointerWidget::~FramePointerWidget() {
	delete d_ui;
}
