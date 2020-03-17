#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include <fort/studio/bridge/IdentifierBridge.hpp>

#include <QDebug>
#include <QSortFilterProxyModel>

AntListWidget::AntListWidget(QWidget * parent)
	: QWidget(parent)
	, d_ui ( new Ui::AntListWidget)
	, d_identifier(NULL)
	, d_sortedModel(new QSortFilterProxyModel(this)) {

	d_ui->setupUi(this);

	updateNumber();

	d_ui->colorBox->setEnabled(false);
	d_ui->colorBox->setCurrentIndex(-1);
	d_ui->filterEdit->setEnabled(false);
	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);
	d_ui->showAllButton->setEnabled(false);
	d_ui->unsoloAllButton->setEnabled(false);

	d_ui->tableView->setModel(d_sortedModel);
	auto header = d_ui->tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);

	connect(d_ui->tableView,
	        &QTableView::doubleClicked,
	        this,
	        &AntListWidget::onDoubleClicked);

	d_sortedModel->setFilterKeyColumn(0);
	connect(d_ui->filterEdit,
	        &QLineEdit::textChanged,
	        d_sortedModel,
	        static_cast<void (QSortFilterProxyModel::*)(const QString &)>(&QSortFilterProxyModel::setFilterRegExp));
}

AntListWidget::~AntListWidget() {
	delete d_ui;
}

void AntListWidget::setup(IdentifierBridge * identifier) {
	d_identifier = identifier;
	updateNumber();
	d_ui->filterEdit->clear();
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

	connect(d_identifier->antModel(),
	        &QAbstractItemModel::rowsInserted,
	        this,
	        &AntListWidget::updateNumber);

	connect(d_identifier->antModel(),
	        &QAbstractItemModel::rowsRemoved,
	        this,
	        &AntListWidget::updateNumber);

	connect(d_ui->showAllButton,
	        &QPushButton::clicked,
	        d_identifier,
	        &IdentifierBridge::showAll);

	connect(d_ui->unsoloAllButton,
	        &QPushButton::clicked,
	        d_identifier,
	        &IdentifierBridge::unsoloAll);

	connect(d_identifier,
	        &IdentifierBridge::numberHiddenAntChanged,
	        this,
	        &AntListWidget::updateShowAll);

	connect(d_identifier,
	        &IdentifierBridge::numberSoloAntChanged,
	        this,
	        &AntListWidget::updateShowAll);

	connect(d_identifier,
	        &IdentifierBridge::numberSoloAntChanged,
	        this,
	        &AntListWidget::updateUnsoloAll);

	updateShowAll();
	updateUnsoloAll();

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
	auto header = d_ui->tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AntListWidget::on_deleteButton_clicked() {
	const auto & sortedSelection = d_ui->tableView->selectionModel()->selection();

	auto selected = d_sortedModel->mapSelectionToSource(sortedSelection);

	if (selected.isEmpty() == true ) {
		return;
	}

	d_identifier->deleteSelection(selected);
}


void AntListWidget::onDoubleClicked(const QModelIndex & index) {
	d_identifier->selectAnt(d_sortedModel->mapToSource(d_sortedModel->index(index.row(),0)));
}


void AntListWidget::updateNumber() {
	size_t n = 0;
	if ( d_identifier != NULL ) {
		n = d_identifier->antModel()->rowCount();
	}
	d_ui->antLabel->setText(tr("Number: %1").arg(n));
}

void AntListWidget::updateShowAll() {
	bool enabled = d_identifier != NULL && (d_identifier->numberSoloAnt() > 0
	                                        || d_identifier->numberHiddenAnt() > 0);
	d_ui->showAllButton->setEnabled(enabled);
}

void AntListWidget::updateUnsoloAll() {
	bool enabled = d_identifier != NULL && d_identifier->numberSoloAnt() > 0;
	d_ui->unsoloAllButton->setEnabled(enabled);
}
