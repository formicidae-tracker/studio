#pragma once

#include <QWidget>

namespace Ui {
class IdentificationEditorWidget;
}

class ExperimentBridge;
class SelectedIdentificationBridge;

class IdentificationEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit IdentificationEditorWidget(QWidget *parent = 0);
	~IdentificationEditorWidget();

	void setup(ExperimentBridge * experiment);

private:
	Ui::IdentificationEditorWidget * d_ui;
	SelectedIdentificationBridge   * d_selectedIdentification;
};
