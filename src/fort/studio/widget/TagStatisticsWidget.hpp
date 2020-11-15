#ifndef TAGSTATISTICSWIDGET_HPP
#define TAGSTATISTICSWIDGET_HPP

#include <QWidget>

namespace Ui {
class TagStatisticsWidget;
}

class TagStatisticsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TagStatisticsWidget(QWidget *parent = 0);
	~TagStatisticsWidget();

private:
	Ui::TagStatisticsWidget *ui;
};

#endif // TAGSTATISTICSWIDGET_HPP
