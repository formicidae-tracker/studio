#pragma once

#include <QWidget>

namespace Ui {
class ZonesEditorWidget;
}

class ZoneBridge;

class ZonesEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit ZonesEditorWidget(QWidget *parent = 0);
	virtual ~ZonesEditorWidget();

	void setup(ZoneBridge * zones);

private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();


private:
	Ui::ZonesEditorWidget * d_ui;

	ZoneBridge * d_zones;
};
