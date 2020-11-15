#include "TagStatisticsWidget.hpp"
#include "ui_TagStatisticsWidget.h"

TagStatisticsWidget::TagStatisticsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TagStatisticsWidget)
{
	ui->setupUi(this);
}

TagStatisticsWidget::~TagStatisticsWidget()
{
	delete ui;
}
