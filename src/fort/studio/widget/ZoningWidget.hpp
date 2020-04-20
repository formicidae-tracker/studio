#pragma once

#include <QWidget>

namespace Ui {
class ZoningWidget;
}

class ZoningWidget : public QWidget {
	Q_OBJECT
public:
	explicit ZoningWidget(QWidget *parent = 0);
	~ZoningWidget();

private:
	Ui::ZoningWidget * d_ui;
};
