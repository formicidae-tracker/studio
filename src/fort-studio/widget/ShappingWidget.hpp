#pragma once

#include <QWidget>

namespace Ui {
class ShappingWidget;
}

class ExperimentBridge;
class QStandardItemModel;

class ShappingWidget : public QWidget {
	Q_OBJECT
public:
	explicit ShappingWidget(QWidget *parent = 0);
	~ShappingWidget();

	void setup(ExperimentBridge * experiment);



private slots:
	void on_toolBox_currentChanged(int);

	void onAntSelected(bool);
protected:
	void changeEvent(QEvent * event) override;
private:
	void setShappingMode();
	void setMeasureMode();

	void buildCloseUpList();

	Ui::ShappingWidget * d_ui;
	ExperimentBridge   * d_experiment;
	QStandardItemModel * d_closeUps;
};
