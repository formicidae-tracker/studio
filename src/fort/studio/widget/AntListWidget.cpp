#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntDisplayBridge.hpp>

#include <QDebug>
#include <QSortFilterProxyModel>

AntListWidget::AntListWidget(QWidget * parent)
	: QWidget(parent)
	, d_ui ( new Ui::AntListWidget)
	, d_experiment(nullptr)
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

void AntListWidget::setup(ExperimentBridge * experiment) {
	d_experiment = experiment;
	updateNumber();
	d_ui->filterEdit->clear();
	d_sortedModel->setSourceModel(d_experiment->antDisplay()->model());
	auto header = d_ui->tableView->horizontalHeader();
	header->setSortIndicatorShown(true);
	header->setSortIndicator(0,Qt::AscendingOrder);
	header->setSortIndicatorShown(true);
	connect(d_experiment->antDisplay(),
	        &AntDisplayBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	connect(d_experiment->antDisplay(),
	        &AntDisplayBridge::activated,
	        d_ui->filterEdit,
	        &QLineEdit::setEnabled);

	connect(d_ui->tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &AntListWidget::onSelectionChanged);

	connect(d_experiment->antDisplay()->model(),
	        &QAbstractItemModel::rowsInserted,
	        this,
	        &AntListWidget::updateNumber);

	connect(d_experiment->antDisplay()->model(),
	        &QAbstractItemModel::rowsRemoved,
	        this,
	        &AntListWidget::updateNumber);

	connect(d_ui->showAllButton,
	        &QPushButton::clicked,
	        d_experiment->antDisplay(),
	        &AntDisplayBridge::showAll);

	connect(d_ui->unsoloAllButton,
	        &QPushButton::clicked,
	        d_experiment->antDisplay(),
	        &AntDisplayBridge::unsoloAll);

	connect(d_experiment->antDisplay(),
	        &AntDisplayBridge::numberHiddenAntChanged,
	        this,
	        &AntListWidget::updateShowAll);

	connect(d_experiment->antDisplay(),
	        &AntDisplayBridge::numberSoloAntChanged,
	        this,
	        &AntListWidget::updateShowAll);

	connect(d_experiment->antDisplay(),
	        &AntDisplayBridge::numberSoloAntChanged,
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
	d_ui->deleteButton->setEnabled(selection->selectedRows().size() == 1);
}


void AntListWidget::on_colorBox_colorChanged(const QColor & color) {
	if ( color.isValid() == false ) {
		return;
	}

	auto rows = d_ui->tableView->selectionModel()->selectedRows();

	if (rows.isEmpty() == true ) {
		return;
	}
	for ( const auto & index : rows ) {
		auto antID = d_experiment->antDisplay()->antIDForIndex(d_sortedModel->mapToSource(index));
		d_experiment->antDisplay()->setAntDisplayColor(antID,color);
	}

}

void AntListWidget::on_addButton_clicked() {
	d_experiment->createAnt();
	auto header = d_ui->tableView->horizontalHeader();
	header->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void AntListWidget::on_deleteButton_clicked() {
	auto rows = d_ui->tableView->selectionModel()->selectedRows();

	if (rows.size() != 1 ) {
		return;
	}
	auto antID = d_experiment->antDisplay()->antIDForIndex(d_sortedModel->mapToSource(rows[0]));
	d_experiment->deleteAnt(antID);
}


void AntListWidget::onDoubleClicked(const QModelIndex & index) {
	auto antID = d_experiment->antDisplay()->antIDForIndex(d_sortedModel->mapToSource(index));
	d_experiment->selectAnt(antID);
}


void AntListWidget::updateNumber() {
	size_t n = 0;
	if ( d_experiment != nullptr ) {
		n = d_experiment->antDisplay()->model()->rowCount();
	}
	d_ui->antLabel->setText(tr("Number: %1").arg(n));
}

void AntListWidget::updateShowAll() {
	bool enabled = d_experiment != nullptr && (d_experiment->antDisplay()->numberSoloAnt() > 0
	                                        || d_experiment->antDisplay()->numberHiddenAnt() > 0);
	d_ui->showAllButton->setEnabled(enabled);
}

void AntListWidget::updateUnsoloAll() {
	bool enabled = d_experiment != nullptr && d_experiment->antDisplay()->numberSoloAnt() > 0;
	d_ui->unsoloAllButton->setEnabled(enabled);
}
