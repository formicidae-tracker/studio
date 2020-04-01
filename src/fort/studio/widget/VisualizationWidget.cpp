#include "VisualizationWidget.hpp"
#include "ui_VisualizationWidget.h"

#include <fort/studio/bridge/MovieBridge.hpp>

VisualizationWidget::VisualizationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VisualizationWidget) {
	d_ui->setupUi(this);
}

VisualizationWidget::~VisualizationWidget() {
	delete d_ui;
}

void VisualizationWidget::setup(MovieBridge * bridge) {
	d_ui->treeView->setModel(bridge->movieModel());
	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	connect(bridge->movieModel(),
	        &QAbstractItemModel::rowsInserted,
	        d_ui->treeView,
	        &QTreeView::expandAll);
	d_ui->treeView->expandAll();

}
