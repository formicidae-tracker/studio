#include "TagStatisticsWidget.hpp"
#include "ui_TagStatisticsWidget.h"

#include <fort/studio/Format.hpp>


TagStatisticsWidget::TagStatisticsWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TagStatisticsWidget) {
	d_ui->setupUi(this);
	clear();
}

TagStatisticsWidget::~TagStatisticsWidget() {
	delete d_ui;
}


void TagStatisticsWidget::clear() {
	d_ui->countLabel->setText(tr("N.A."));
	d_ui->multipleCountLabel->setText(tr("N.A."));
	d_ui->firstSeenLabel->setText(tr("N.A."));
	d_ui->lastSeenLabel->setText(tr("N.A."));
}

void TagStatisticsWidget::display(fm::TagID tagID,
                                  fm::TagStatistics stats,size_t frameCount) {
	double ratio = double(stats.Counts(0,0)) / double(frameCount) * 1000.0;

	d_ui->countLabel->setText(QString::number(stats.Counts(0,0))
	                          + " ("
	                          + QString::number(ratio,'f',2) + " ‰)");

	d_ui->multipleCountLabel->setText(QString::number(stats.Counts(1,0)));

	d_ui->firstSeenLabel->setText(ToQString(stats.FirstSeen));
	d_ui->lastSeenLabel->setText(ToQString(stats.LastSeen));

}
