#pragma once

#include <QWidget>

namespace Ui {
class GlobalPropertyWidget;
}

class ExperimentBridge;

class GlobalPropertyWidget : public QWidget {
	Q_OBJECT
public:
	explicit GlobalPropertyWidget(QWidget *parent);
	virtual ~GlobalPropertyWidget();

	void setup(ExperimentBridge *experiment);

private:
	friend class GlobalPropertyUTest_WidgetTest_Test;
	Ui::GlobalPropertyWidget * d_ui;
};
