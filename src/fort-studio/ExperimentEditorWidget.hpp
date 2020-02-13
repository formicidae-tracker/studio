#pragma once

#include <QWidget>


namespace Ui {
class ExperimentEditorWidget;
}

class ExperimentBridge;

class ExperimentEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit ExperimentEditorWidget(QWidget *parent);
	virtual ~ExperimentEditorWidget();

	void setup(ExperimentBridge *experiment);

private:
	Ui::ExperimentEditorWidget * d_ui;
};
