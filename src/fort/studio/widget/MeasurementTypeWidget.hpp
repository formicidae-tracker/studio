#pragma once

#include <QWidget>


namespace Ui {
class MeasurementTypeWidget;
}

class MeasurementBridge;

class MeasurementTypeWidget : public QWidget {
	Q_OBJECT
public:
	explicit MeasurementTypeWidget(QWidget *parent = nullptr);
	virtual ~MeasurementTypeWidget();

	void setup(MeasurementBridge * measurements);

public slots:
	void onSelectionChanged();

	void on_addButton_clicked();
	void on_deleteButton_clicked();

private:
	friend class MeasurementUTest_TypeWidgetTest_Test;
	Ui::MeasurementTypeWidget * d_ui;
	MeasurementBridge         * d_measurements;
};
