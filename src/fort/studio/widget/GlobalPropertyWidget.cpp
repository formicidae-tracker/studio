#include "GlobalPropertyWidget.hpp"
#include "ui_GlobalPropertyWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <QDoubleSpinBox>

#include <QAction>

GlobalPropertyWidget::GlobalPropertyWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::GlobalPropertyWidget) {
	d_ui->setupUi(this);

	d_ui->familySelector->setEnabled(false);
	d_ui->tagSizeEdit->setEnabled(false);
	d_ui->nameEdit->setEnabled(false);
	d_ui->authorEdit->setEnabled(false);
	d_ui->commentEdit->setEnabled(false);

}

GlobalPropertyWidget::~GlobalPropertyWidget() {
	delete d_ui;
}

void GlobalPropertyWidget::setup(ExperimentBridge * experiment,
                                 QAction * loadTagCloseUpAction) {
	auto properties = experiment->globalProperties();
	connect(properties,
	        &GlobalPropertyBridge::activated,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setEnabled);


	connect(properties,
	        &GlobalPropertyBridge::activated,
	        d_ui->tagSizeEdit,
	        &QDoubleSpinBox::setEnabled);

	connect(properties,
	        &GlobalPropertyBridge::tagSizeChanged,
	        d_ui->tagSizeEdit,
	        &QDoubleSpinBox::setValue);

	connect(d_ui->tagSizeEdit,
	        static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
	        properties,
	        &GlobalPropertyBridge::setTagSize);



	connect(properties,
	        &GlobalPropertyBridge::activated,
	        d_ui->nameEdit,
	        &QLineEdit::setEnabled);

	connect(properties,
	        &GlobalPropertyBridge::nameChanged,
	        d_ui->nameEdit,
	        &QLineEdit::setText);

	connect(d_ui->nameEdit,
	        &QLineEdit::textChanged,
	        properties,
	        &GlobalPropertyBridge::setName);



	connect(properties,
	        &GlobalPropertyBridge::activated,
	        d_ui->authorEdit,
	        &QLineEdit::setEnabled);

	connect(properties,
	        &GlobalPropertyBridge::authorChanged,
	        d_ui->authorEdit,
	        &QLineEdit::setText);

	connect(d_ui->authorEdit,
	        &QLineEdit::textChanged,
	        properties,
	        &GlobalPropertyBridge::setAuthor);



	connect(properties,
	        &GlobalPropertyBridge::activated,
	        d_ui->commentEdit,
	        &QLineEdit::setEnabled);

	connect(properties,
	        &GlobalPropertyBridge::commentChanged,
	        d_ui->commentEdit,
	        &QPlainTextEdit::setPlainText);

	connect(d_ui->commentEdit,
	        &QPlainTextEdit::textChanged,
	        [properties,this]() {
		        properties->setComment(d_ui->commentEdit->toPlainText(),true);
	        });


	connect(experiment->measurements(),
	        &MeasurementBridge::progressChanged,
	        [this](size_t done,size_t toDo) {
	        });

	connect(loadTagCloseUpAction,&QAction::changed,
	        this,[this,loadTagCloseUpAction]() {
		             d_ui->familySelector->setEnabled(loadTagCloseUpAction->isEnabled());
	             });

}
