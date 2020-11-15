#include "TagStatisticsWorkspace.hpp"
#include "ui_TagStatisticsWorkspace.h"

#include <QSortFilterProxyModel>

#include <fort/studio/bridge/ExperimentBridge.hpp>

TagStatisticsWorkspace::TagStatisticsWorkspace(QWidget *parent)
	: Workspace(false,parent)
	, d_ui(new Ui::TagStatisticsWorkspace) {
	d_ui->setupUi(this);


}

TagStatisticsWorkspace::~TagStatisticsWorkspace() {
	delete d_ui;
}


void TagStatisticsWorkspace::initialize(QMainWindow * main,ExperimentBridge * experiment) {
	auto sorted = new QSortFilterProxyModel(this);
	sorted->setSourceModel(experiment->statistics()->stats());
	sorted->setSortRole(Qt::UserRole+1);
	d_ui->tableView->setModel(sorted);

	d_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void TagStatisticsWorkspace::setUp(const NavigationAction & actions) {

}

void TagStatisticsWorkspace::tearDown(const NavigationAction & actions) {

}
