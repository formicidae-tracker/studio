#pragma once

#include <QWidget>

namespace Ui {
class UserDataWidget;
}

class UserDataWidget : public QWidget {
	Q_OBJECT
public:
	explicit UserDataWidget(QWidget *parent = 0);
	~UserDataWidget();

private:
	Ui::UserDataWidget * d_ui;
};
