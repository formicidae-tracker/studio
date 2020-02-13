#include "ExperimentEditorWidget.hpp"
#include "ui_ExperimentEditorWidget.h"

#include "ExperimentBridge.hpp"
#include <QDoubleSpinBox>


ExperimentEditorWidget::ExperimentEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ExperimentEditorWidget) {
	d_ui->setupUi(this);

	d_ui->familySelector->setEnabled(false);
	d_ui->tagSizeEdit->setEnabled(false);
	d_ui->nameEdit->setEnabled(false);
	d_ui->authorEdit->setEnabled(false);
	d_ui->commentEdit->setEnabled(false);

}

ExperimentEditorWidget::~ExperimentEditorWidget() {
	delete d_ui;
}

void ExperimentEditorWidget::setup(ExperimentBridge *experiment) {
	connect(experiment,
	        &ExperimentBridge::activated,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setEnabled);

	connect(experiment,
	        &ExperimentBridge::tagFamilyChanged,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setFamily);

	connect(d_ui->familySelector,
	        &TagFamilyComboBox::familyModified,
	        experiment,
	        &ExperimentBridge::setTagFamily);



	connect(experiment,
	        &ExperimentBridge::activated,
	        d_ui->tagSizeEdit,
	        &QDoubleSpinBox::setEnabled);

	connect(experiment,
	        &ExperimentBridge::tagSizeChanged,
	        d_ui->tagSizeEdit,
	        &QDoubleSpinBox::setValue);

	connect(d_ui->tagSizeEdit,
	        static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	        experiment,
	        &ExperimentBridge::setTagSize);



	connect(experiment,
	        &ExperimentBridge::activated,
	        d_ui->nameEdit,
	        &QLineEdit::setEnabled);

	connect(experiment,
	        &ExperimentBridge::nameChanged,
	        d_ui->nameEdit,
	        &QLineEdit::setText);

	connect(d_ui->nameEdit,
	        &QLineEdit::textChanged,
	        experiment,
	        &ExperimentBridge::setName);



	connect(experiment,
	        &ExperimentBridge::activated,
	        d_ui->authorEdit,
	        &QLineEdit::setEnabled);

	connect(experiment,
	        &ExperimentBridge::authorChanged,
	        d_ui->authorEdit,
	        &QLineEdit::setText);

	connect(d_ui->authorEdit,
	        &QLineEdit::textChanged,
	        experiment,
	        &ExperimentBridge::setAuthor);



	connect(experiment,
	        &ExperimentBridge::activated,
	        d_ui->commentEdit,
	        &QLineEdit::setEnabled);

	connect(experiment,
	        &ExperimentBridge::commentChanged,
	        d_ui->commentEdit,
	        &QPlainTextEdit::setPlainText);

	connect(d_ui->commentEdit,
	        &QPlainTextEdit::textChanged,
	        [experiment,this]() {
		        experiment->setComment(d_ui->commentEdit->toPlainText());
	        });

}
