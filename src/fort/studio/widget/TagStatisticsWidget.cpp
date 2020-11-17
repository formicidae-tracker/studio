#include "TagStatisticsWidget.hpp"
#include "ui_TagStatisticsWidget.h"

#include <fort/studio/Format.hpp>

#include <QChart>
#include <QBarSet>

TagStatisticsWidget::TagStatisticsWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TagStatisticsWidget) {
	d_ui->setupUi(this);

	d_chart = new QChart();

	d_xAxis = new QBarCategoryAxis();
	d_xAxis->append({"&lt;0.5s","&lt;1s","&lt;10s","&lt;1m","&lt;10m","&lt;1h","&lt;10h","≥10h"});
	d_xAxis->setLabelsVisible(true);
	d_chart->addAxis(d_xAxis,Qt::AlignBottom);

	d_yAxis = new QValueAxis();
	d_chart->addAxis(d_yAxis,Qt::AlignLeft);
	d_yAxis->setLabelsVisible(true);
	d_yAxis->setLabelFormat("%d");
	d_yAxis->setRange(0,100);
	d_yAxis->setTickCount(3);


	d_chart->legend()->setVisible(false);
	d_chart->setMargins(QMargins(0,0,0,0));


	d_chart->layout()->setContentsMargins(0,0,0,0);

	d_ui->chartView->setRubberBand(QChartView::NoRubberBand);

	d_ui->chartView->setChart(d_chart);

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
	d_yAxis->setRange(0,100);
	d_yAxis->setTickCount(3);
	d_yAxis->applyNiceNumbers();

	d_chart->removeAllSeries();
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

	d_chart->removeAllSeries();

	auto barSet = new QBarSet(tr("Tag %1").arg(fm::FormatTagID(tagID).c_str()));
	std::vector<int> rowIndexes = { fm::TagStatistics::GAP_500MS,
	                                fm::TagStatistics::GAP_1S,
	                                fm::TagStatistics::GAP_10S,
	                                fm::TagStatistics::GAP_1M,
	                                fm::TagStatistics::GAP_10M,
	                                fm::TagStatistics::GAP_1H,
	                                fm::TagStatistics::GAP_10H,
	                                fm::TagStatistics::GAP_MORE
	};
	int maxValue = 0;
	for ( const auto & index : rowIndexes ) {
		int v = stats.Counts(index);
		barSet->append(v);
		maxValue = std::max(maxValue,v);
	}

	d_yAxis->setRange(0,maxValue);
	d_yAxis->setTickCount(3);
	d_yAxis->applyNiceNumbers();

	auto series = new QBarSeries();
	series->append(barSet);

	d_chart->addSeries(series);

	series->attachAxis(d_xAxis);

	series->attachAxis(d_yAxis);


	d_ui->chartView->fitInView(d_chart);

}
