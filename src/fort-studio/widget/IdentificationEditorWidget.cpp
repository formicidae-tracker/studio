#include "IdentificationEditorWidget.hpp"
#include "ui_IdentificationEditorWidget.h"

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


void IdentificationEditorWidget::setup(SelectedIdentificationBridge * selectedIdentification) {
	d_selectedIdentification = selectedIdentification;

	connect(selectedIdentification,
	        &SelectedIdentificationBridge::activated,
	        d_ui->useGlobalSizeBox,
	        &QCheckBox::setEnabled);

	connect(selectedIdentification,
	        &SelectedIdentificationBridge::useGlobalSizeChanged,
	        d_ui->useGlobalSizeBox,
	        &QAbstractButton::setChecked);

	connect(d_ui->useGlobalSizeBox,
	        &QAbstractButton::toggled,
	        selectedIdentification,
	        &SelectedIdentificationBridge::setUseGlobalSize);


	connect(d_ui->useGlobalSizeBox,
	        &QAbstractButton::toggled,
	        [this](bool checked) {
		        d_ui->sizeBox->setEnabled(!checked);
	        });

	connect(selectedIdentification,
	        &SelectedIdentificationBridge::tagSizeChanged,
	        d_ui->sizeBox,
	        &QDoubleSpinBox::setValue);

	connect(d_ui->sizeBox,
	        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	        selectedIdentification,
	        &SelectedIdentificationBridge::setTagSize);

}
