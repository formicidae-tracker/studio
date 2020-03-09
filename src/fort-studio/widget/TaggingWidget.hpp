#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include <myrmidon/priv/ForwardDeclaration.hpp>

class GlobalPropertyBridge;
class MeasurementBridge;
class IdentifierBridge;
class QSortFilterProxyModel;
class VectorialScene;
class Vector;

namespace fmp = fort::myrmidon::priv;

namespace Ui {
class TaggingWidget;
}

class TaggingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TaggingWidget(QWidget *parent = 0);
	~TaggingWidget();


	void setup(GlobalPropertyBridge * globalProperties,
	           MeasurementBridge * measurements,
	           IdentifierBridge * identifier);


public slots:
	void on_addIdentButton_clicked();
	void on_newAntButton_clicked();
	void on_deletePoseButton_clicked();

	void on_treeView_activated(const QModelIndex & index);

	void onIdentificationAntPositionChanged(fmp::IdentificationConstPtr);

	void onVectorUpdated();
	void onVectorCreated(Vector * vector);
	void onVectorRemoved();

	void setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu);

	void nextTag();
	void nextTagCloseUp();
	void previousTag();
	void previousTagCloseUp();

private:
	void selectRow(int tagRow, int tcuRow);
	bool eventFilter(QObject * obj, QEvent * event);

	Ui::TaggingWidget     * d_ui;
	QSortFilterProxyModel * d_sortedModel;
	MeasurementBridge     * d_measurements;
	IdentifierBridge      * d_identifier;
	VectorialScene        * d_vectorialScene;
	fmp::TagCloseUpConstPtr d_tcu;
};
