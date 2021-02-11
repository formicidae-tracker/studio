#include "CloneShapeDialog.hpp"
#include "ui_CloneShapeDialog.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>

#include <fort/studio/Format.hpp>

#include <QDebug>



CloneShapeDialog::CloneShapeDialog(ExperimentBridge * experiment,
                                   quint32 sourceID,
                                   QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::CloneShapeDialog) {
	d_ui->setupUi(this);
	auto ant = experiment->ant(sourceID);
	auto antIDStr = fmp::Ant::FormatID(sourceID).c_str();

	if ( ant == nullptr ) {
		throw std::runtime_error("Could not find ant " + std::string(antIDStr) );
	}


	setWindowModality(Qt::ApplicationModal);

	setWindowTitle(tr("Clone shape from Ant %1").arg(antIDStr));

	d_ui->listingBox->setTitle(tr("Shapes Summary for Ant %1").arg(antIDStr));

	std::map<fmp::AntShapeType::ID,size_t> counts;
	auto types = experiment->antShapeTypes()->types();
	for ( const auto & [stID,type] : types ) {
		counts.insert(std::make_pair(stID,0));
	}

	for ( const auto & [stID,c] : ant->Capsules() ) {
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
		dialog = std::make_shared<CloneShapeDialog>(experiment,experiment->selectedAntID(),parent);
	} catch ( const std::exception & e ) {
		qCritical() << e.what();
		return nullptr;
	}
	QEventLoop loop;
	connect(dialog.get(), &QDialog::finished,
	        &loop,&QEventLoop::quit);
	dialog->open();
	loop.exec();
	if ( dialog->result() == QDialog::Rejected ) {
		return nullptr;
	}
	return std::make_shared<CloneOptions>(CloneOptions{.OverwriteShapes = dialog->d_ui->overwriteBox->checkState() == Qt::Checked,
	                                                   .ScaleToSize = dialog->d_ui->scaleBox->checkState() == Qt::Checked
		});
}
