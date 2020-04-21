#include "ZonesEditorWidget.hpp"
#include "ui_ZonesEditorWidget.h"

#include <fort/studio/bridge/ZoneBridge.hpp>

ZonesEditorWidget::ZonesEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ZonesEditorWidget)
	, d_zones(nullptr) {
	d_ui->setupUi(this);
	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
}

ZonesEditorWidget::~ZonesEditorWidget() {
	delete d_ui;
}


void ZonesEditorWidget::setup(ZoneBridge * zones) {
	d_zones = zones;
	d_ui->treeView->setModel(zones->spaceModel());
	d_ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	auto sModel = d_ui->treeView->selectionModel();
	connect(sModel,&QItemSelectionModel::selectionChanged,
	        this,
	        [this,sModel]() {
		        if ( sModel->hasSelection() == false ) {
			        d_ui->addButton->setEnabled(false);
			        d_ui->removeButton->setEnabled(false);
			        d_zones->activateItem(QModelIndex());
		        }
		        auto index = sModel->selectedIndexes()[0];
		        d_ui->addButton->setEnabled(d_zones->canAddItemAt(index));
		        d_ui->removeButton->setEnabled(d_zones->canRemoveItemAt(index));
		        d_zones->activateItem(index);
	        });

	connect(d_zones->spaceModel(), &QAbstractItemModel::rowsInserted,
	        this,
	        [this]() {
		        d_ui->treeView->expandAll();
	        });

}

void ZonesEditorWidget::on_addButton_clicked() {
	auto sModel = d_ui->treeView->selectionModel();
	if ( d_zones == nullptr || sModel->hasSelection() == false ) {
		return;
	}
	auto index = sModel->selectedIndexes()[0];
	d_zones->addItemAtIndex(index);
}

void ZonesEditorWidget::on_removeButton_clicked() {
	auto sModel = d_ui->treeView->selectionModel();
	if ( d_zones == nullptr || sModel->hasSelection() == false ) {
		return;
	}
	auto index = sModel->selectedIndexes()[0];
	d_zones->removeItemAtIndex(index);

}
