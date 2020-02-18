#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include "IdentifierBridge.hpp"

#include <QDebug>
#include <QSortFilterProxyModel>

AntListWidget::AntListWidget(QWidget * parent)
	: QWidget(parent)
	, d_ui ( new Ui::AntListWidget)
	, d_identifier(NULL)
	, d_sortedModel(new QSortFilterProxyModel(this)) {

	d_ui->setupUi(this);

	d_ui->colorBox->setEnabled(false);
	d_ui->colorBox->setCurrentIndex(-1);
	d_ui->filterEdit->setEnabled(false);
	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);

	d_ui->tableView->setModel(d_sortedModel);
	auto header = d_ui->tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);

	connect(d_ui->tableView,
	        &QTableView::doubleClicked,
	        this,
	        &AntListWidget::onDoubleClicked);
}

AntListWidget::~AntListWidget() {
	delete d_ui;
}

void AntListWidget::setup(IdentifierBridge * identifier) {
	d_identifier = identifier;
	d_sortedModel->setSourceModel(d_identifier->antModel());
	auto header = d_ui->tableView->horizontalHeader();
	header->setSortIndicatorShown(true);
	header->setSortIndicator(0,Qt::AscendingOrder);
	header->setSortIndicatorShown(true);
	connect(d_identifier,
	        &IdentifierBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	connect(d_identifier,
	        &IdentifierBridge::activated,
	        d_ui->filterEdit,
	        &QLineEdit::setEnabled);

	connect(d_ui->tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &AntListWidget::onSelectionChanged);

}


void AntListWidget::onSelectionChanged() {
	const auto selection  = d_ui->tableView->selectionModel();
	if ( selection->hasSelection() == false ) {
		d_ui->deleteButton->setEnabled(false);
		d_ui->colorBox->setCurrentIndex(-1);
		d_ui->colorBox->setEnabled(false);
		return;
	}
	d_ui->colorBox->setCurrentIndex(-1);
	d_ui->colorBox->setEnabled(true);
	d_ui->deleteButton->setEnabled(true);
}


void AntListWidget::on_colorBox_colorChanged(const QColor & color) {
	if ( color.isValid() == false ) {
		return;
	}

	const auto & sortedSelection = d_ui->tableView->selectionModel()->selection();

	auto selected = d_sortedModel->mapSelectionToSource(sortedSelection);

	if (selected.isEmpty() == true ) {
		return;
	}

	d_identifier->setAntDisplayColor(selected,color);

}

void AntListWidget::on_addButton_clicked() {
	d_identifier->createAnt();
}

void AntListWidget::on_deleteButton_clicked() {



}


void AntListWidget::onDoubleClicked(const QModelIndex & index) {
	d_identifier->selectAnt(d_sortedModel->mapToSource(d_sortedModel->index(index.row(),0)));
}
