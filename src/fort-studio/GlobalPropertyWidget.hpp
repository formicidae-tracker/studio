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
	Ui::GlobalPropertyWidget * d_ui;
};
