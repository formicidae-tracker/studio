#include "UniverseEditorWidget.hpp"
#include "ui_UniverseEditorWidget.h"

#include "UniverseBridge.hpp"

#include <QDebug>


UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UniverseEditorWidget) {
	d_ui->setupUi(this);

	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);

	connect(d_ui->treeView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &UniverseEditorWidget::onSelectionChanged);
}

UniverseEditorWidget::~UniverseEditorWidget() {
	delete d_ui;
}


void UniverseEditorWidget::setup(UniverseBridge * universe) {
	d_universe = universe;
	connect(d_universe,
	        &UniverseBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	d_ui->treeView->setModel(d_universe->model());

}


void UniverseEditorWidget::on_addButton_clicked() {
	if ( d_universe == NULL ) {
		return;
	}

	qWarning() << "Implements me !";
}

void UniverseEditorWidget::on_deleteButton_clicked() {
	if ( !d_universe ) {
		return;
	}

	auto selected = d_ui->treeView->selectionModel()->selectedIndexes();
	if ( selected.isEmpty() ) {
		return;
	}

	d_universe->deleteSelection(selected);
}


void UniverseEditorWidget::onSelectionChanged(const QItemSelection & selection) {
	if ( selection.isEmpty() ) {
		d_ui->deleteButton->setEnabled(false);
		return;
	}
	bool enabled = true;
	for ( const auto & index : selection.indexes() ) {
		if ( d_universe->isDeletable(index) == false ) {
			enabled = false;
			break;
		}
	}
	d_ui->deleteButton->setEnabled(enabled);
}
