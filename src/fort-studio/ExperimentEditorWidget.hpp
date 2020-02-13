#pragma once

#include <QWidget>

#include "ExperimentController.hpp"


namespace Ui {
class ExperimentEditorWidget;
}

class UniverseBridge;

class ExperimentEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit ExperimentEditorWidget(QWidget *parent);

	void setup(UniverseBridge *,
	           ExperimentBridge *);

signals:

private:
	Ui::ExperimentEditorWidget * d_ui;

	UniverseBridge           * d_zones;
};
