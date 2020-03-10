#pragma once

#include <QWidget>

namespace Ui {
class IdentificationEditorWidget;
}

class SelectedIdentificationBridge;

class IdentificationEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit IdentificationEditorWidget(QWidget *parent = 0);
	~IdentificationEditorWidget();

	void setup(SelectedIdentificationBridge * selectedIdentification);

private:
	Ui::IdentificationEditorWidget * d_ui;
	SelectedIdentificationBridge   * d_selectedIdentification;
};
