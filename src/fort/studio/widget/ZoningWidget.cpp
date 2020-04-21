#include "ZoningWidget.hpp"
#include "ui_ZoningWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <QClipboard>


#include <fort/studio/Format.hpp>


ZoningWidget::ZoningWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ZoningWidget)
	, d_zones(nullptr) {
	d_ui->setupUi(this);
}

ZoningWidget::~ZoningWidget() {
	delete d_ui;
}

void ZoningWidget::setup(ExperimentBridge * experiment) {
	d_zones = experiment->zones();
	d_ui->zonesEditor->setup(d_zones);
	d_ui->listView->setModel(d_zones->fullFrameModel());
	d_ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
	auto sModel = d_ui->listView->selectionModel();
	connect(sModel,&QItemSelectionModel::selectionChanged,
	        this,
	        [this,sModel]() {
		        if ( sModel->hasSelection() == false ) {
			        display(nullptr);
		        }
		        auto f = d_zones->fullFrameAtIndex(sModel->selectedIndexes()[0]);
		        if ( f.first == false ) {
			        display(nullptr);
		        } else {
			        display(std::make_shared<ZoneBridge::FullFrame>(f.second));
		        }
	        });

}


void ZoningWidget::display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe) {
	d_fullframe = fullframe;
	if ( d_copyAction != nullptr ) {
		d_copyAction->setEnabled(!fullframe == false);
	}
}


void ZoningWidget::setUp(const NavigationAction & actions) {
	connect(actions.NextCloseUp,&QAction::triggered,
	        this,&ZoningWidget::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        this,&ZoningWidget::previousCloseUp);

	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&ZoningWidget::onCopyTime);

	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);
	actions.CopyCurrentTime->setEnabled(!d_fullframe == false);
	d_copyAction = actions.CopyCurrentTime;
}

void ZoningWidget::tearDown(const NavigationAction & actions) {
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           this,&ZoningWidget::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           this,&ZoningWidget::previousCloseUp);

	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&ZoningWidget::onCopyTime);

	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyAction = nullptr;
}


void ZoningWidget::nextCloseUp() {
	select(+1);
}

void ZoningWidget::previousCloseUp() {
	select(-1);
}

void ZoningWidget::onCopyTime() {
	if ( !d_fullframe == true ) {
		return;
	}
	QApplication::clipboard()->setText(ToQString(d_fullframe->Reference.Time()));
}


void ZoningWidget::select(int increment) {
	if ( d_zones->fullFrameModel()->rowCount() == 0 || increment == 0 ) {
		return;
	}
	auto model = d_zones->fullFrameModel();
	auto sModel = d_ui->listView->selectionModel();
	auto rows = sModel->selectedRows();
	if ( rows.isEmpty() == true ) {
		if ( increment > 0 ) {
			sModel->select(model->index(0,0),QItemSelectionModel::ClearAndSelect);
		} else {
			sModel->select(model->index(model->rowCount()-1,0),QItemSelectionModel::ClearAndSelect);
		}
		return;
	}

	int currentRow = rows[0].row();
	int newRow = std::clamp(currentRow + increment,0,model->rowCount()-1);
	if ( currentRow == newRow ) {
		return;
	}
	sModel->select(model->index(newRow,0),QItemSelectionModel::ClearAndSelect);
}
