#pragma once

#include <QWidget>

namespace Ui {
class TimeEditorWidget;
}

class TimeEditorWidget : public QWidget {
	Q_OBJECT

public:
	explicit TimeEditorWidget(QWidget *parent = 0);
	~TimeEditorWidget();

private:
	Ui::TimeEditorWidget * d_ui;
};
