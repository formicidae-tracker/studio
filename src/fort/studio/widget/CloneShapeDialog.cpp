#include "CloneShapeDialog.hpp"
#include "ui_CloneShapeDialog.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <fort/studio/Format.hpp>

CloneShapeDialog::CloneShapeDialog(ExperimentBridge * experiment,
                                   QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::CloneShapeDialog) {
	d_ui->setupUi(this);
	auto ant = experiment->selectedAnt();
	if ( ant->isActive() == false ) {
		throw std::runtime_error("No selected ant");
	}

	auto antIDStr = ToQString(fmp::Ant::FormatID(ant->selectedID()));

	setWindowModality(Qt::ApplicationModal);

	setWindowTitle(tr("Clone shape from Ant %1").arg(antIDStr));

	d_ui->listingBox->setTitle(tr("Shapes Summary for Ant %1").arg(antIDStr));

	std::map<fmp::AntShapeType::ID,size_t> counts;
	auto types = experiment->antShapeTypes()->types();
	for ( const auto & [stID,type] : types ) {
		counts.insert(std::make_pair(stID,0));
	}

	for ( const auto & [stID,c] : ant->capsules() ) {
		counts[stID] += 1;
	}

	d_ui->tableWidget->setColumnCount(3);
	d_ui->tableWidget->setRowCount(3);
	int row = 0;
	for ( const auto & [stID,type] : types ) {
		auto idItem = new QTableWidgetItem(QString::number(stID));
		idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
		auto nameItem = new QTableWidgetItem(type->Name().c_str());
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
		auto countItem = new QTableWidgetItem(QString::number(counts.at(stID)));
		countItem->setFlags(countItem->flags() & ~Qt::ItemIsEditable);
		d_ui->tableWidget->setItem(row,0,idItem);
		d_ui->tableWidget->setItem(row,1,nameItem);
		d_ui->tableWidget->setItem(row,2,countItem);
		++row;
	}
	d_ui->tableWidget->setHorizontalHeaderLabels({tr("ShapeTypeID"),tr("Name"),tr("Count")});
	d_ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

CloneShapeDialog::~CloneShapeDialog() {
	delete d_ui;
}


std::shared_ptr<CloneShapeDialog::CloneOptions>
CloneShapeDialog::get(ExperimentBridge * experiment,
                      QWidget *parent) {
	std::shared_ptr<CloneShapeDialog> dialog;
	try {
		dialog = std::make_shared<CloneShapeDialog>(experiment,parent);
	} catch ( const std::exception & e ) {
		qCritical() << e.what();
		return {};
	}
	QEventLoop loop;
	connect(dialog.get(), &QDialog::finished,
	        &loop,&QEventLoop::quit);
	dialog->open();
	loop.exec();
	if ( dialog->result() == QDialog::Rejected ) {
		return {};
	}
	return std::make_shared<CloneOptions>(CloneOptions{.OverwriteShapes = dialog->d_ui->overwriteBox->checkState() == Qt::Checked,
	                                                   .ScaleToSize = dialog->d_ui->scaleBox->checkState() == Qt::Checked
		});
}
