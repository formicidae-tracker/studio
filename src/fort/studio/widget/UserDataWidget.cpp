#include "UserDataWidget.hpp"
#include "ui_UserDataWidget.h"

#include <fort/studio/bridge/AntMetadataBridge.hpp>

UserDataWidget::UserDataWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UserDataWidget) {
	d_ui->setupUi(this);
}

UserDataWidget::~UserDataWidget() {
	delete d_ui;
}


void UserDataWidget::setup(AntMetadataBridge * metadata) {
	d_metadata = metadata;

	d_ui->metadataEditor->setup(metadata);

	d_ui->tableView->setModel(metadata->dataModel());
	d_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}
