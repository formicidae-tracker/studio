#include "UserDataWidget.hpp"
#include "ui_UserDataWidget.h"

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/AntMetadataBridge.hpp>

#include <QDebug>

UserDataWidget::UserDataWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UserDataWidget) {
	d_ui->setupUi(this);
	d_ui->removeButton->setEnabled(false);
	onSelectedAntID(0);
}

UserDataWidget::~UserDataWidget() {
	delete d_ui;
}


void UserDataWidget::setup(AntMetadataBridge * metadata) {
	d_metadata = metadata;

	d_ui->metadataEditor->setup(metadata);

	d_ui->dataView->setModel(metadata->dataModel());
	d_ui->dataView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	d_ui->timeView->setModel(metadata->timedChangeModel());
	d_ui->timeView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	d_ui->comboBox->setModel(metadata->columnModel());

	connect(d_metadata,
	        &AntMetadataBridge::selectedAntIDChanged,
	        this,
	        &UserDataWidget::onSelectedAntID);
	d_ui->timeView->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(d_ui->timeView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        d_ui->removeButton->setEnabled(d_ui->timeView->selectionModel()->hasSelection());
	        });

	connect(d_ui->addButton,
	        &QToolButton::clicked,
	        this,
	        &UserDataWidget::onAddButtonClicked);

	connect(d_ui->removeButton,
	        &QToolButton::clicked,
	        this,
	        &UserDataWidget::onRemoveButtonClicked);

}


void UserDataWidget::on_dataView_activated(const QModelIndex & index) {
	d_metadata->selectRow(index.row());
}

void UserDataWidget::onSelectedAntID(quint32 ID) {
	if ( ID == 0 ) {
		d_ui->addButton->setEnabled(false);
		d_ui->comboBox->setEnabled(false);
		d_ui->timeBox->setTitle(tr("Timed Changes - No Ant Selected"));

		return;
	}
	d_ui->addButton->setEnabled(true);
	d_ui->comboBox->setEnabled(true);
	d_ui->timeBox->setTitle(tr("Timed Changes for Ant %1").arg(ToQString(fmp::Ant::FormatID(ID))));
}


void UserDataWidget::onAddButtonClicked() {
	auto column = d_ui->comboBox->currentData(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
	if (!column || d_metadata->selectedAntID() == 0 ) {
		return;
	}

	d_metadata->addTimedChange(d_metadata->selectedAntID(),
	                           ToQString(column->Name()));
	d_ui->timeView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void UserDataWidget::onRemoveButtonClicked() {
	auto sModel = d_ui->timeView->selectionModel();
	qWarning() << "coucou " << sModel->selectedIndexes();
	if ( sModel->hasSelection() == false ) {
		return;
	}
	d_metadata->removeTimedChange(sModel->selectedIndexes()[0]);
}
