#pragma once

#include <QWidget>


namespace Ui {
class GlobalPropertyWidget;
}

class GlobalPropertyBridge;

class GlobalPropertyWidget : public QWidget {
	Q_OBJECT
public:
	explicit GlobalPropertyWidget(QWidget *parent);
	virtual ~GlobalPropertyWidget();

	void setup(GlobalPropertyBridge *properties);

private:
	friend class GlobalPropertyUTest_WidgetTest_Test;
	Ui::GlobalPropertyWidget * d_ui;
};
