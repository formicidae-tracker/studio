#include "IdentificationEditorWidget.hpp"
#include "ui_IdentificationEditorWidget.h"

#include <fort-studio/bridge/ExperimentBridge.hpp>
#include <fort-studio/bridge/SelectedIdentificationBridge.hpp>

#include <QCheckBox>

IdentificationEditorWidget::IdentificationEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::IdentificationEditorWidget)
	, d_selectedIdentification(nullptr) {
	d_ui->setupUi(this);
}

IdentificationEditorWidget::~IdentificationEditorWidget() {
	delete d_ui;
}


void IdentificationEditorWidget::setup(ExperimentBridge * experiment) {
	d_selectedIdentification = experiment->selectedIdentification();

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::activated,
	        d_ui->useGlobalSizeBox,
	        &QCheckBox::setEnabled);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::useGlobalSizeChanged,
	        d_ui->useGlobalSizeBox,
	        &QAbstractButton::setChecked);

	connect(d_ui->useGlobalSizeBox,
	        &QAbstractButton::toggled,
	        d_selectedIdentification,
	        &SelectedIdentificationBridge::setUseGlobalSize);


	connect(d_ui->useGlobalSizeBox,
	        &QAbstractButton::toggled,
	        [this](bool checked) {
		        d_ui->sizeBox->setEnabled(!checked);
	        });

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::tagSizeChanged,
	        d_ui->sizeBox,
	        &QDoubleSpinBox::setValue);

	connect(d_ui->sizeBox,
	        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	        d_selectedIdentification,
	        &SelectedIdentificationBridge::setTagSize);

	d_ui->startTime->setup(experiment->universe());
	d_ui->endTime->setup(experiment->universe());

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::activated,
	        d_ui->startTime,
	        &TimeEditorWidget::setEnabled);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::startModified,
	        d_ui->startTime,
	        &TimeEditorWidget::setTime);

	connect(d_ui->startTime,
	        &TimeEditorWidget::timeChanged,
	        d_selectedIdentification,
	        &SelectedIdentificationBridge::setStart);


	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::activated,
	        d_ui->endTime,
	        &TimeEditorWidget::setEnabled);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::endModified,
	        d_ui->endTime,
	        &TimeEditorWidget::setTime);

	connect(d_ui->endTime,
	        &TimeEditorWidget::timeChanged,
	        d_selectedIdentification,
	        &SelectedIdentificationBridge::setEnd);


}
