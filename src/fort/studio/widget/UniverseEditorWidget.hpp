#pragma once


#include <QWidget>

class QItemSelection;
class UniverseBridge;

namespace Ui {
class UniverseEditorWidget;
}


class UniverseEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit UniverseEditorWidget(QWidget *parent = 0);
	~UniverseEditorWidget();

	void setup(UniverseBridge * universe);

public slots:

	void on_addButton_clicked();
	void on_deleteButton_clicked();

	void onSelectionChanged(const QItemSelection &);

private:
	friend class UniverseUTest_WidgetTest_Test;
	Ui::UniverseEditorWidget * d_ui;
	UniverseBridge           * d_universe;
};
