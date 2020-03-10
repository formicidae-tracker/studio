#include "AntIdentificationWidget.hpp"
#include "ui_AntIdentificationWidget.h"

#include <fort-studio/bridge/SelectedAntBridge.hpp>
#include <fort-studio/Format.hpp>

AntIdentificationWidget::AntIdentificationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntIdentificationWidget)
	, d_selectedAnt(nullptr) {
	d_ui->setupUi(this);
	auto hHeader = d_ui->tableView->horizontalHeader();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	hHeader->setStretchLastSection(true);
	d_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

AntIdentificationWidget::~AntIdentificationWidget() {
	delete d_ui;
}


void AntIdentificationWidget::setup(SelectedAntBridge * selectedAnt) {
	d_selectedAnt = selectedAnt;
	connect(d_selectedAnt,
	        &SelectedAntBridge::activated,
	        this,
	        &AntIdentificationWidget::onSelection);
	d_ui->tableView->setModel(d_selectedAnt->identificationModel());
	d_ui->identificationEditor->setup(d_selectedAnt->selectedIdentification());
}


void  AntIdentificationWidget::onSelection() {
	if ( d_selectedAnt->isActive() == false ) {
		d_ui->groupBox->setTitle(tr("Selected Ant: None"));
		return;
	}

	d_ui->groupBox->setTitle(tr("Selected Ant: %1").arg(ToQString(fmp::Ant::FormatID(d_selectedAnt->selectedID()))));

}

void AntIdentificationWidget::on_tableView_doubleClicked(const QModelIndex & index) {
	d_selectedAnt->selectIdentification(index);
}
