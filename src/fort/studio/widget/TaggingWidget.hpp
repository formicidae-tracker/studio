#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include <fort/myrmidon/priv/ForwardDeclaration.hpp>

#include "Navigatable.hpp"

class ExperimentBridge;
class GlobalPropertyBridge;
class MeasurementBridge;
class IdentifierBridge;
class SelectedAntBridge;
class QSortFilterProxyModel;
class VectorialScene;
class Vector;

namespace fmp = fort::myrmidon::priv;

namespace Ui {
class TaggingWidget;
}

class QAction;

class TaggingWidget : public QWidget, public Navigatable {
    Q_OBJECT
public:
    explicit TaggingWidget(QWidget *parent = 0);
	~TaggingWidget();

	void setup(ExperimentBridge * experiment,
	           QAction * loadTagCloseUpAction);

	QAction * newAntFromTagAction() const;
	QAction * addIdentificationToAntAction() const;
	QAction * deletePoseEstimationAction() const;

public slots:
	void on_treeView_activated(const QModelIndex & index);

	void onIdentificationAntPositionChanged(fmp::IdentificationConstPtr);
	void onIdentificationDeleted(fmp::IdentificationConstPtr);

	void onVectorUpdated();
	void onVectorCreated(QSharedPointer<Vector> vector);
	void onVectorRemoved();

	void nextTag();
	void nextTagCloseUp();
	void previousTag();
	void previousTagCloseUp();
protected:
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;
private slots:
	void setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu);

	void addIdentification();
	void newAnt();
	void deletePose();

	void onCopyTime();


	void updateActionStates();


private:
	void selectRow(int tagRow, int tcuRow);

	Ui::TaggingWidget     * d_ui;
	QSortFilterProxyModel * d_sortedModel;
	MeasurementBridge     * d_measurements;
	IdentifierBridge      * d_identifier;
	VectorialScene        * d_vectorialScene;
	SelectedAntBridge     * d_selectedAnt;
	fmp::TagCloseUpConstPtr d_tcu;
	QAction               * d_newAntAction,*d_addIdentificationAction,*d_deletePoseAction;
	QAction               * d_copyTimeAction;
};
