#ifndef EXPERIMENTINFOWIDGET_HPP
#define EXPERIMENTINFOWIDGET_HPP

#include <QWidget>

namespace Ui {
class ExperimentInfoWidget;
}

class ExperimentInfoWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ExperimentInfoWidget(QWidget *parent = nullptr);
	~ExperimentInfoWidget();

private:
	Ui::ExperimentInfoWidget *ui;
};

#endif // EXPERIMENTINFOWIDGET_HPP
