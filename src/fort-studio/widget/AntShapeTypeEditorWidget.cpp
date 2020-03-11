#include "AntShapeTypeEditorWidget.hpp"
#include "ui_AntShapeTypeEditorWidget.h"

#include <fort-studio/bridge/AntShapeTypeBridge.hpp>

AntShapeTypeEditorWidget::AntShapeTypeEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntShapeTypeEditorWidget)
	, d_shapeTypes(nullptr) {
	d_ui->setupUi(this);

	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);


}

AntShapeTypeEditorWidget::~AntShapeTypeEditorWidget() {
	delete d_ui;
}

void AntShapeTypeEditorWidget::setup(AntShapeTypeBridge * shapeTypes) {
	d_shapeTypes = shapeTypes;

	connect(shapeTypes,
	        &AntShapeTypeBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	d_ui->tableView->setModel(shapeTypes->shapeModel());

	auto hHeader = d_ui->tableView->horizontalHeader();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);


	connect(d_ui->tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        d_ui->removeButton->setEnabled(d_ui->tableView->selectionModel()->hasSelection());
	        });

	d_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);


}


void AntShapeTypeEditorWidget::on_addButton_clicked() {
	if ( d_shapeTypes == nullptr ) {
		return;
	}

	d_shapeTypes->addType("");
}

void AntShapeTypeEditorWidget::on_removeButton_clicked() {
	auto rows = d_ui->tableView->selectionModel()->selectedRows(1);
	if ( rows.isEmpty() == true ) {
		return;
	}
	d_shapeTypes->deleteType(rows[0].data(Qt::DisplayRole).toInt());
}
