#include "FrameFinderDialog.hpp"
#include "ui_FrameFinderDialog.h"

FrameFinderDialog::FrameFinderDialog(UniverseBridge * universe,
                                     QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::FrameFinderDialog) {
	d_ui->setupUi(this);
}

FrameFinderDialog::~FrameFinderDialog() {
	delete d_ui;
}


fmp::FrameReference::ConstPtr FrameFinderDialog::Get(UniverseBridge * universe,
                                                     QWidget * parent) {
	return fmp::FrameReference::ConstPtr();
}
