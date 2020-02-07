#include "ExperimentEditorWidget.hpp"
#include "ui_ExperimentEditorWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QListWidget>

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include "ZoneModel.hpp"

ExperimentEditorWidget::ExperimentEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ExperimentEditorWidget)
	, d_controller(NULL)
	, d_zones(new ZoneModel(this)) {
	d_ui->setupUi(this);

	d_ui->zoneView->setModel(d_zones->model());
	d_ui->zoneView->expandAll();
	d_ui->zoneView->resizeColumnToContents(0);
	d_ui->zoneView->resizeColumnToContents(1);
	d_ui->zoneView->resizeColumnToContents(2);
	d_ui->zoneView->resizeColumnToContents(3);
	d_ui->zoneView->resizeColumnToContents(4);
	d_ui->zoneView->resizeColumnToContents(5);

	d_ui->familySelector->insertItem(0,"36h11",(int)fort::tags::Family::Tag36h11);
    d_ui->familySelector->insertItem(1,"36ARTag",(int)fort::tags::Family::Tag36ARTag);
    d_ui->familySelector->insertItem(2,"36h10",(int)fort::tags::Family::Tag36h10);
    d_ui->familySelector->insertItem(3,"Standard41h12",(int)fort::tags::Family::Standard41h12);
    d_ui->familySelector->insertItem(4,"16h5",(int)fort::tags::Family::Tag16h5);
    d_ui->familySelector->insertItem(5,"25h9",(int)fort::tags::Family::Tag25h9);
    d_ui->familySelector->insertItem(6,"Circle21h7",(int)fort::tags::Family::Circle21h7);
    d_ui->familySelector->insertItem(7,"Circle49h12",(int)fort::tags::Family::Circle49h12);
    d_ui->familySelector->insertItem(8,"Custom48h12",(int)fort::tags::Family::Custom48h12);
    d_ui->familySelector->insertItem(9,"Standard52h13",(int)fort::tags::Family::Standard52h13);
    d_ui->familySelector->setCurrentIndex(-1);

	onNewController(NULL);
}

ExperimentEditorWidget::~ExperimentEditorWidget() {
	delete d_ui;
}

void ExperimentEditorWidget::onNewController(ExperimentController * controller) {

	if ( d_controller != NULL ) {
		disconnect(d_controller,
		           SIGNAL(nameChanged(const QString &)),
		           d_ui->nameEdit,
		           SLOT(setText(const QString &)));

		disconnect(d_controller,
		           SIGNAL(authorChanged(const QString &)),
		           d_ui->authorEdit,
		           SLOT(setText(const QString &)));

		disconnect(d_controller,
		           SIGNAL(commentChanged(const QString &)),
		           d_ui->commentEdit,
		           SLOT(setText(const QString &)));

		disconnect(d_controller,
		           SIGNAL(tagSizeChanged(double)),
		           d_ui->tagSizeEdit,
		           SLOT(setValue(double)));

		disconnect(d_controller,
		           SIGNAL(tagFamilyChanged(fort::tags::Family)),
		           this,
		           SLOT(onTagFamilyChanged(fort::tags::Family)));
	}

	d_controller = controller;

	if (d_controller == NULL) {

		d_ui->nameEdit->clear();
		d_ui->nameEdit->setEnabled(false);
		d_ui->authorEdit->clear();
		d_ui->authorEdit->setEnabled(false);
		d_ui->commentEdit->clear();
		d_ui->commentEdit->setEnabled(false);
		d_ui->tagSizeEdit->clear();
		d_ui->tagSizeEdit->setEnabled(false);
		d_ui->familySelector->setCurrentIndex(-1);
		d_ui->familySelector->setEnabled(false);
		return;
	}

	connect(d_controller,
	        SIGNAL(nameChanged(const QString &)),
	        d_ui->nameEdit,
	        SLOT(setText(const QString &)));
	d_ui->nameEdit->setText(d_controller->name());
	d_ui->nameEdit->setEnabled(true);
	connect(d_controller,
	        SIGNAL(authorChanged(const QString &)),
	        d_ui->authorEdit,
	        SLOT(setText(const QString &)));
	d_ui->authorEdit->setText(d_controller->author());
	d_ui->authorEdit->setEnabled(true);
	connect(d_controller,
	        SIGNAL(commentChanged(const QString &)),
	        d_ui->commentEdit,
	        SLOT(setText(const QString &)));
	d_ui->commentEdit->setPlainText(d_controller->comment());
	d_ui->commentEdit->setEnabled(true);
	connect(d_controller,
	        SIGNAL(tagSizeChanged(double)),
	        d_ui->tagSizeEdit,
	        SLOT(setValue(double)));
	d_ui->tagSizeEdit->setValue(d_controller->tagSize());
	d_ui->tagSizeEdit->setEnabled(true);
	connect(d_controller,
	        SIGNAL(tagFamilyChanged(fort::tags::Family)),
	        this,
	        SLOT(onFamilyChanged(fort::tags::Family)));
	onFamilyChanged(d_controller->tagFamily());
	d_ui->familySelector->setEnabled(true);
}


void ExperimentEditorWidget::on_nameEdit_textEdited(const QString & text) {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setName(text);
}

void ExperimentEditorWidget::on_authorEdit_textEdited(const QString & text) {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setAuthor(text);
}

void ExperimentEditorWidget::on_commentEdit_textChanged() {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setComment(d_ui->commentEdit->toPlainText());
}

void ExperimentEditorWidget::on_familySelector_currentIndexChanged(int index) {
	if (index < 0 || d_controller == NULL) {
		return;
	}
	d_controller->setTagFamily(fort::tags::Family(d_ui->familySelector->itemData(index).toInt()));
}

void ExperimentEditorWidget::on_tagSizeEdit_valueChanged(double value ) {
	if ( d_controller == NULL ) {
		return;
	}
	d_controller->setTagSize(value);
}

void ExperimentEditorWidget::onFamilyChanged(fort::tags::Family f) {
	if (d_controller == NULL) {
		return;
	}
	int index= d_ui->familySelector->findData((int)f);
	d_ui->familySelector->setCurrentIndex(index);
}
