#include "AntEditorWidget.hpp"
#include "ui_AntEditorWidget.h"

#include <fort-studio/bridge/ExperimentBridge.hpp>
#include <fort-studio/Format.hpp>

#include <QStandardItemModel>

AntEditorWidget::AntEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntEditorWidget)
	, d_closeUps(new QStandardItemModel(this) ) {
	d_ui->setupUi(this);
	d_ui->treeView->setModel(d_closeUps);
	auto hHeader = d_ui->treeView->header();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
}

AntEditorWidget::~AntEditorWidget() {
	delete d_ui;
}

#include <iostream>

void AntEditorWidget::setup(ExperimentBridge * experiment) {
	d_experiment = experiment;

	d_ui->shapeTypeEditor->setup(d_experiment->antShapeTypes());
	d_ui->measurementTypeEditor->setup(d_experiment->measurements());


	connect(d_experiment->selectedAnt(),
	        &SelectedAntBridge::activated,
	        this,
	        &AntEditorWidget::onAntSelected);

	auto mTypeModel = d_experiment->measurements()->measurementTypeModel();
	connect(mTypeModel,
	        &QAbstractItemModel::rowsRemoved,
	        [this](const QModelIndex & parent, int row, int count) {
		        buildCloseUpList();
	        });

	connect(mTypeModel,
	        &QAbstractItemModel::rowsInserted,
	        [this](const QModelIndex & parent, int row, int count) {
		        buildCloseUpList();
	        });


	d_ui->toolBox->setCurrentIndex(0);
	on_toolBox_currentChanged(0);

}

void  AntEditorWidget::on_toolBox_currentChanged(int index) {
	switch(index) {
	case 0:
		setShappingMode();
		break;
	case 1:
		setMeasureMode();
		break;
	default:
		qWarning() << "Inconsistent tab size for Shapping widget";
	}
}


void AntEditorWidget::setShappingMode() {
	d_ui->comboBox->setModel(d_experiment->antShapeTypes()->shapeModel());
}

void AntEditorWidget::setMeasureMode() {
	d_ui->comboBox->setModel(d_experiment->measurements()->measurementTypeModel());
}

void AntEditorWidget::onAntSelected(bool antSelected) {
	if ( isEnabled() == false ) {
		return;
	}
	buildCloseUpList();
}


void AntEditorWidget::changeEvent(QEvent * event)  {
	QWidget::changeEvent(event);
	if ( event->type() == QEvent::EnabledChange && isEnabled() == true ) {
		buildCloseUpList();
	}
}

void AntEditorWidget::buildHeaders() {
	auto measurementTypes = d_experiment->measurements()->measurementTypeModel();
	QStringList labels;
	labels.reserve(1 + measurementTypes->rowCount());
	labels.push_back(tr("Name"));
	for ( size_t i = 0; i < measurementTypes->rowCount(); ++i ) {
		auto name = measurementTypes->data(measurementTypes->index(i,0),Qt::DisplayRole).toString();
		labels.push_back(tr("%1 count").arg(name));
	}
	d_closeUps->setHorizontalHeaderLabels(labels);
}

void AntEditorWidget::buildCloseUpList() {
	d_closeUps->clear();



	if ( d_experiment->selectedAnt()->isActive() == false ) {
		return;
	}
	auto formatedAntID = fmp::Ant::FormatID(d_experiment->selectedAnt()->selectedID());

	auto measurementTypes = d_experiment->measurements()->measurementTypeModel();
	buildHeaders();

	std::map<quint32,int> mTypeIDs;
	for ( size_t i = 0; i < measurementTypes->rowCount(); ++i ) {
		mTypeIDs.insert(std::make_pair(measurementTypes->data(measurementTypes->index(i,1),Qt::DisplayRole).toInt(),0));
	}

	auto ant = new QStandardItem(QString("Ant %1").arg(formatedAntID.c_str()));
	ant->setData(QVariant::fromValue(fmp::TagCloseUp::ConstPtr()));
	ant->setEditable(false);

	QVector<fmp::TagCloseUp::ConstPtr> tcus;
	for ( const auto & ident : d_experiment->selectedAnt()->identifications() ) {
		d_experiment->measurements()->queryTagCloseUp(tcus,ident);
	}

	std::sort(tcus.begin(),tcus.end(),
	          [](const fmp::TagCloseUp::ConstPtr & a,
	             const fmp::TagCloseUp::ConstPtr & b) -> bool {
		          return a->Frame().Time().Before(b->Frame().Time());
	          });

	for ( const auto & tcu : tcus ) {
		QList<QStandardItem*> newRow;
		newRow.reserve(1 + measurementTypes->rowCount());
		auto tcuItem = new QStandardItem(ToQString(tcu->URI()));
		auto data = QVariant::fromValue(tcu);
		tcuItem->setData(data);
		tcuItem->setEditable(false);
		newRow.append(tcuItem);

		for ( auto & [mTypeID,count] : mTypeIDs ) {
			auto m = d_experiment->measurements()->measurement(tcu->URI(),
			                                                   mTypeID);


			auto colItem = new QStandardItem(!m ? "" : "1");
			colItem->setData(data);
			colItem->setEditable(false);

			if ( m ) {
				++count;
			}
			newRow.append(colItem);
		}
		ant->appendRow(newRow);
	}

	QList<QStandardItem*> antRow({ant});
	antRow.reserve(1 + measurementTypes->rowCount());
	for ( const auto & [mTypeID,count] : mTypeIDs ) {
		auto countItem = new QStandardItem(QString::number(count));
		countItem->setEditable(false);
		countItem->setData(QVariant::fromValue(fmp::TagCloseUp::ConstPtr()));
		antRow.push_back(countItem);
	}

	d_closeUps->appendRow(antRow);
	d_ui->treeView->expand(d_closeUps->index(0,0));

}


void AntEditorWidget::on_treeView_activated(const QModelIndex & index) {
	auto item = d_closeUps->itemFromIndex(index);
	if ( d_experiment == nullptr || item == nullptr ) {
		return;
	}
	auto tcu = item->data().value<fmp::TagCloseUp::ConstPtr>();
	if ( !tcu ) {
		return;
	}

}
