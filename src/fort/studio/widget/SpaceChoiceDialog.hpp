#pragma once

#include <QDialog>

namespace Ui {
class SpaceChoiceDialog;
}

class UniverseBridge;

class SpaceChoiceDialog : public QDialog {
	Q_OBJECT
public:
	explicit SpaceChoiceDialog(UniverseBridge * universe,
	                           QWidget *parent = nullptr);
	virtual ~SpaceChoiceDialog();

	static QString Get(UniverseBridge * universe,
	                   QWidget * parent);

private:
	Ui::SpaceChoiceDialog * d_ui;
};
