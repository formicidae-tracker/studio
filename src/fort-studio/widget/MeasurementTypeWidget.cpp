#include "MeasurementTypeWidget.hpp"
#include "ui_MeasurementTypeWidget.h"

#include <fort-studio/bridge/MeasurementBridge.hpp>

MeasurementTypeWidget::MeasurementTypeWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::MeasurementTypeWidget)
	, d_measurements(NULL) {
	d_ui->setupUi(this);
	d_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);
}

MeasurementTypeWidget::~MeasurementTypeWidget() {
	delete d_ui;
}

void MeasurementTypeWidget::setup(MeasurementBridge * measurements) {
	d_measurements = measurements;
	d_ui->tableView->setModel(measurements->measurementTypeModel());
	connect(d_measurements,
	        &MeasurementBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);
	connect(d_ui->tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &MeasurementTypeWidget::onSelectionChanged);
}


void MeasurementTypeWidget::onSelectionChanged() {
	auto selection = d_ui->tableView->selectionModel();
	if ( selection->hasSelection() == false
	     || selection->isRowSelected(0,QModelIndex()) == true ) {
		d_ui->deleteButton->setEnabled(false);
		return;
	}
	d_ui->deleteButton->setEnabled(true);
}

void MeasurementTypeWidget::on_addButton_clicked() {
	d_measurements->setMeasurementType(-1,"new-measurement-type");
}

void MeasurementTypeWidget::on_deleteButton_clicked() {
	for ( const auto & index : d_ui->tableView->selectionModel()->selectedRows() ) {
		if ( index.row() == 0 ) {
			continue;
		}
		d_measurements->deleteMeasurementType(index);
	}
}
