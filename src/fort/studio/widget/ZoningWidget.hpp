#pragma once

#include <QWidget>

namespace Ui {
class ZoningWidget;
}
class ExperimentBridge;

class ZoningWidget : public QWidget {
	Q_OBJECT
public:
	explicit ZoningWidget(QWidget *parent = 0);
	~ZoningWidget();

	void setup(ExperimentBridge * experiment);

private:
	Ui::ZoningWidget * d_ui;
};
