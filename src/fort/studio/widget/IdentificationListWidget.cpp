#include "IdentificationListWidget.hpp"
#include "ui_IdentificationListWidget.h"

#include <QSortFilterProxyModel>

#include <fort/studio/bridge/IdentifierBridge.hpp>

IdentificationListWidget::IdentificationListWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::IdentificationListWidget)
	, d_sortedModel(new QSortFilterProxyModel) {
	d_ui->setupUi(this);


	d_ui->identificationsView->setModel(d_sortedModel);

	connect(d_ui->identificationFilterEdit,
	        &QLineEdit::textChanged,
	        d_sortedModel,
	        static_cast<void(QSortFilterProxyModel::*)(const QString &)>(&QSortFilterProxyModel::setFilterRegExp));

	d_ui->removeButton->setEnabled(false);

}

IdentificationListWidget::~IdentificationListWidget() {
	delete d_ui;
}


void IdentificationListWidget::initialize(IdentifierBridge * identifier) {
	d_identifier = identifier;

	connect(d_identifier,
	        &IdentifierBridge::activated,
	        d_ui->identificationFilterEdit,
	        &QLineEdit::clear);

	d_sortedModel->setSourceModel(identifier->identificationsModel());
	d_ui->identificationsView->setSortingEnabled(true);
	d_ui->identificationsView->sortByColumn(0,Qt::AscendingOrder);

	connect(d_ui->identificationsView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &IdentificationListWidget::onIdentificationsViewSelectionChanged);

	//always sorts nicely identifications
	connect(d_identifier,
	        &IdentifierBridge::activated,
	        this,
	        [this]() {
		        d_ui->identificationsView->horizontalHeader()->setSortIndicatorShown(true);
		        d_ui->identificationsView->sortByColumn(0,Qt::AscendingOrder);
	        });
}

void IdentificationListWidget::on_identificationsView_activated(const QModelIndex & index) {
	if (index.column() != 0 ) {
		return;
	}

	auto identification = d_identifier->identificationForIndex(d_sortedModel->mapToSource(index));
	if ( !identification == true ) {
		return;
	}
	emit identificationSelected(identification);
}

void IdentificationListWidget::onIdentificationsViewSelectionChanged() {
	auto selectedRows = d_ui->identificationsView->selectionModel()->selectedRows();
	d_ui->removeButton->setEnabled(selectedRows.size() == 1 );
}

void IdentificationListWidget::on_removeButton_clicked() {
	auto selectedRows = d_ui->identificationsView->selectionModel()->selectedRows();
	if ( selectedRows.size() != 1 ) {
		return;
	}

	auto identification = d_identifier->identificationForIndex(d_sortedModel->mapToSource(selectedRows[0]));
	if ( !identification == true ) {
		return;
	}
	d_identifier->deleteIdentification(identification);
}
