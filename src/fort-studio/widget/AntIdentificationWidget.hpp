#pragma once

#include <QWidget>

namespace Ui {
class AntIdentificationWidget;
}

class SelectedAntBridge;


class AntIdentificationWidget : public QWidget {
	Q_OBJECT
public:
	explicit AntIdentificationWidget(QWidget *parent = 0);
	~AntIdentificationWidget();

	void setup(SelectedAntBridge * selectedAnt);
private slots :
	void onSelection();
	void on_tableView_doubleClicked(const QModelIndex & index);
private:
	Ui::AntIdentificationWidget * d_ui;

	SelectedAntBridge * d_selectedAnt;

};
