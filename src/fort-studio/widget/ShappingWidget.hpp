#pragma once

#include <QWidget>

namespace Ui {
class ShappingWidget;
}

class ExperimentBridge;

class ShappingWidget : public QWidget {
	Q_OBJECT
public:
	explicit ShappingWidget(QWidget *parent = 0);
	~ShappingWidget();

	void setup(ExperimentBridge * experiment);


private:
	Ui::ShappingWidget * d_ui;
};
