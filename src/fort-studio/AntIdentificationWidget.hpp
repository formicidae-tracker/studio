#pragma once

#include <QWidget>

namespace Ui {
class AntIdentificationWidget;
}

class AntIdentificationWidget : public QWidget {
	Q_OBJECT
public:
	explicit AntIdentificationWidget(QWidget *parent = 0);
	~AntIdentificationWidget();

private:
	Ui::AntIdentificationWidget * d_ui;
};
