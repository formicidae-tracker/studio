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
}

AntEditorWidget::~AntEditorWidget() {
	delete d_ui;
}


void AntEditorWidget::setup(ExperimentBridge * experiment) {
	d_experiment = experiment;

	d_ui->shapeTypeEditor->setup(d_experiment->antShapeTypes());
	d_ui->measurementTypeEditor->setup(d_experiment->measurements());


	connect(d_experiment->selectedAnt(),
	        &SelectedAntBridge::activated,
	        this,
	        &AntEditorWidget::onAntSelected);

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

void AntEditorWidget::buildCloseUpList() {
	d_closeUps->clear();
	d_closeUps->setHorizontalHeaderLabels({tr("Name")});

	if ( d_experiment->selectedAnt()->isActive() == false ) {
		return;
	}
	auto formatedAntID = fmp::Ant::FormatID(d_experiment->selectedAnt()->selectedID());

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
		auto tcuItem = new QStandardItem(ToQString(tcu->URI()));
		tcuItem->setData(QVariant::fromValue(tcu));
		tcuItem->setEditable(false);
		ant->appendRow({tcuItem});
	}
	d_closeUps->appendRow(ant);
	d_ui->treeView->expand(d_closeUps->index(0,0));

}
