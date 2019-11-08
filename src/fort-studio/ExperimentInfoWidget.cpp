#include "ExperimentInfoWidget.hpp"
#include "ui_ExperimentInfoWidget.h"

#include <QFileDialog>
#include <QDebug>
#include <QListWidget>


ExperimentInfoWidget::ExperimentInfoWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ExperimentInfoWidget)
	, d_controller(NULL) {
	d_ui->setupUi(this);
	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
	onNewController(NULL);
}


ExperimentInfoWidget::~ExperimentInfoWidget() {
	delete d_ui;
}


void ExperimentInfoWidget::onNewController(ExperimentController * controller) {
	if ( d_controller != NULL ) {
		disconnect(d_controller,
		           SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
		           this,
		           SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));
	}
	d_controller = controller;
	if (d_controller == NULL) {
		d_ui->addButton->setEnabled(false);
		d_ui->removeButton->setEnabled(false);
		d_ui->listWidget->clear();
		d_ui->authorEdit->clear();
		d_ui->authorEdit->setEnabled(false);
		d_ui->nameEdit->clear();
		d_ui->nameEdit->setEnabled(false);
		d_ui->commentEdit->clear();
		d_ui->commentEdit->setEnabled(false);
		return;
	}
	d_ui->addButton->setEnabled(true);
	connect(d_controller,
	        SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
	        this,
	        SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));
	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
	d_ui->nameEdit->setText(d_controller->experiment().Name().c_str());
	d_ui->nameEdit->setEnabled(true);
	d_ui->authorEdit->setText(d_controller->experiment().Author().c_str());
	d_ui->authorEdit->setEnabled(true);
	d_ui->commentEdit->setPlainText(d_controller->experiment().Comment().c_str());
	d_ui->commentEdit->setEnabled(true);
}


void ExperimentInfoWidget::on_addButton_clicked() {
	QString dataDir  = QFileDialog::getExistingDirectory(this, tr("Open Tracking Data Directory"),
	                                                     QFileInfo(d_controller->experiment().AbsolutePath().c_str()).absolutePath(),
	                                                     QFileDialog::ShowDirsOnly);
	if ( dataDir.isEmpty() ) {
		return;
	}
	Error err = d_controller->addDataDirectory(dataDir);
	if (!err.OK()) {
		qWarning() << err.what();
		return;
	}
}

void ExperimentInfoWidget::on_removeButton_clicked() {
	if (d_controller == NULL ) {
		return;
	}
	for(auto const & item : d_ui->listWidget->selectedItems() ) {
		Error err = d_controller->removeDataDirectory(item->text());
		if ( err.OK() == false ) {
			qCritical() << err.what();
		}
	}
}


void ExperimentInfoWidget::onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & tdds) {
	d_ui->listWidget->clear();
	for(auto const & tdd : tdds ) {
		auto item = new QListWidgetItem(tdd.second.LocalPath().c_str(),d_ui->listWidget);
		item->setIcon(QIcon::fromTheme("folder"));
		d_ui->listWidget->addItem(item);
	}
}


void ExperimentInfoWidget::on_nameEdit_textEdited(const QString & text) {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setName(text);
}

void ExperimentInfoWidget::on_authorEdit_textEdited(const QString & text) {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setAuthor(text);
}




void ExperimentInfoWidget::on_commentEdit_textChanged() {
	if(d_controller == NULL) {
		return;
	}
	d_controller->setComment(d_ui->commentEdit->toPlainText());
}


void ExperimentInfoWidget::on_listWidget_itemSelectionChanged() {
	d_ui->removeButton->setEnabled(!d_ui->listWidget->selectedItems().isEmpty());
}
