#pragma once

#include <QWidget>

class AntShapeTypeBridge;

namespace Ui {
class AntShapeTypeEditorWidget;
}

class AntShapeTypeEditorWidget : public QWidget {
	Q_OBJECT

public:
	explicit AntShapeTypeEditorWidget(QWidget *parent = 0);
	~AntShapeTypeEditorWidget();

	void setup(AntShapeTypeBridge * shapeTypes);

public slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();

private:
	friend class AntShapeTypeUTest_AntShapeTypeEditorWidgetTest_Test;
	Ui::AntShapeTypeEditorWidget * d_ui;
	AntShapeTypeBridge           * d_shapeTypes;
};
