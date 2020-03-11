#pragma once

#include <QWidget>

namespace Ui {
class ShappingWidget;
}

class ShappingWidget : public QWidget {
	Q_OBJECT
public:
	explicit ShappingWidget(QWidget *parent = 0);
	~ShappingWidget();

private:
	Ui::ShappingWidget * d_ui;
};
