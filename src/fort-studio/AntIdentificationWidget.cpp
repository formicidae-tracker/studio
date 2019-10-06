#include "AntIdentificationWidget.hpp"
#include "ui_AntIdentificationWidget.h"

#include <QDebug>
#include <myrmidon/priv/FramePointer.hpp>

AntIdentificationWidget::AntIdentificationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntIdentificationWidget) {
	d_ui->setupUi(this);
	d_ui->startFrame->setTitle(tr("Start Frame"));
	d_ui->endFrame->setTitle(tr("End Frame"));
	setEnabled(false);
}

AntIdentificationWidget::~AntIdentificationWidget() {
	delete d_ui;
}


void AntIdentificationWidget::onNewController(ExperimentController * controller) {
	if ( d_controller != NULL ) {

	}
	d_controller = controller;
	d_ui->startFrame->onNewController(controller);
	d_ui->endFrame->onNewController(controller);

}


void AntIdentificationWidget::on_startFrame_framePointerUpdated(const fort::myrmidon::priv::FramePointer::Ptr & frame) {
	if (!frame) {
		qDebug() << "start:  no frame";
		return;
	}
	qDebug() << "start: " << frame->FullPath().c_str();
}

void AntIdentificationWidget::on_endFrame_framePointerUpdated(const fort::myrmidon::priv::FramePointer::Ptr & frame) {
	if (!frame) {
		qDebug() << "end:  no frame";
		return;
	}
	qDebug() << "end: " << frame->FullPath().c_str();

}
