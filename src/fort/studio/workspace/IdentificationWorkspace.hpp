#pragma once

#include <QSortFilterProxyModel>

#include <fort/myrmidon/priv/ForwardDeclaration.hpp>


#include "Workspace.hpp"

class ExperimentBridge;
class GlobalPropertyBridge;
class MeasurementBridge;
class TagCloseUpBridge;
class StatisticsBridge;
class IdentifierBridge;
class SelectedAntBridge;
class VectorialScene;
class Vector;
class QToolBar;
class QToolBarSeparator;

namespace fmp = fort::myrmidon::priv;

namespace Ui {
class IdentificationWorkspace;
}

class QAction;

class CloseUpFilterModel : public QSortFilterProxyModel {
	Q_OBJECT
public:
	CloseUpFilterModel(QObject * parent = nullptr);
	virtual ~CloseUpFilterModel();

public slots:
	void setFilter(const QString & filter);
	void setRemoveUsed(bool removeUsed);
	void setWhiteList(const QString & formattedTagID);
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:

	QRegularExpression d_filter;
	QString            d_whiteList;
	bool               d_removeUsed;
};


class IdentificationWorkspace : public Workspace {
    Q_OBJECT
public:
    explicit IdentificationWorkspace(QWidget *parent = 0);
	virtual ~IdentificationWorkspace();

	QAction * newAntFromTagAction() const;
	QAction * addIdentificationToAntAction() const;
	QAction * deletePoseEstimationAction() const;

public slots:
	void on_closeUpView_clicked(const QModelIndex & index);

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
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
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
	void setGraphicsFromMeasurement(const fmp::TagCloseUpConstPtr & tcu);

	Ui::IdentificationWorkspace * d_ui;
	CloseUpFilterModel          * d_tagSortedModel;
	MeasurementBridge           * d_measurements;
	IdentifierBridge            * d_identifier;
	TagCloseUpBridge            * d_tagCloseUps;
	StatisticsBridge            * d_statistics;
	VectorialScene              * d_vectorialScene;
	SelectedAntBridge           * d_selectedAnt;
	fmp::TagCloseUpConstPtr       d_tcu;
	QAction                     * d_newAntAction,*d_addIdentificationAction,*d_deletePoseAction;
	QAction                     * d_copyTimeAction;
	QToolBar                    * d_actionToolBar,*d_navigationToolBar;
	QToolBarSeparator           * d_toolBarSeparator;
};
