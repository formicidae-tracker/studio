#include "VideoPlayerControl.hpp"
#include "ui_VideoPlayerControl.h"

VideoPlayerControl::VideoPlayerControl(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VideoPlayerControl) {
	d_ui->setupUi(this);
}

VideoPlayerControl::~VideoPlayerControl(){
	delete d_ui;
}
