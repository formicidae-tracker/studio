#pragma once


#include <QWidget>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

class QItemSelection;
class UniverseBridge;

namespace Ui {
class UniverseEditorWidget;
}


class UniverseEditorWidget : public QWidget {
	Q_OBJECT
public:
	explicit UniverseEditorWidget(QWidget *parent = 0);
	virtual ~UniverseEditorWidget();

	void setup(UniverseBridge * universe);

public slots:

	void on_addButton_clicked();
	void on_deleteButton_clicked();

	void onSelectionChanged(const QItemSelection &);


private:
	friend class UniverseUTest_WidgetTest_Test;

	fmp::TrackingDataDirectory::Ptr openTDD(const QString & path);

	Ui::UniverseEditorWidget * d_ui;
	UniverseBridge           * d_universe;



};
