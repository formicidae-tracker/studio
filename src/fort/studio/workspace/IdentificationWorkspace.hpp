#pragma once


#include <fort/myrmidon/priv/ForwardDeclaration.hpp>


#include "Workspace.hpp"

class ExperimentBridge;
class GlobalPropertyBridge;
class MeasurementBridge;
class StatisticsBridge;
class IdentifierBridge;
class VectorialScene;
class Vector;
class QToolBar;
class QToolBarSeparator;
class QDockWidget;
class TagCloseUpExplorer;
class QAction;


namespace fmp = fort::myrmidon::priv;

namespace Ui {
class IdentificationWorkspace;
}


class IdentificationWorkspace : public Workspace {
    Q_OBJECT
public:
    explicit IdentificationWorkspace(QWidget *parent = 0);
	virtual ~IdentificationWorkspace();

	QAction * newAntFromTagAction() const;
	QAction * addIdentificationToAntAction() const;
	QAction * deletePoseEstimationAction() const;

public slots:
	void onIdentificationAntPositionChanged(fmp::IdentificationConstPtr);
	void onIdentificationDeleted(fmp::IdentificationConstPtr);

	void onVectorUpdated();
	void onVectorCreated(QSharedPointer<Vector> vector);
	void onVectorRemoved();

protected:
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;

private slots:
	void setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu);

	void addIdentification();
	void newAnt();
	void deletePose();

	void onTagIDChanged(int tagID);

	void onCopyTime();


	void updateActionStates();

private:
	void setGraphicsFromMeasurement(const fmp::TagCloseUpConstPtr & tcu);

	Ui::IdentificationWorkspace * d_ui;
	MeasurementBridge           * d_measurements;
	IdentifierBridge            * d_identifier;
	StatisticsBridge            * d_statistics;
	VectorialScene              * d_vectorialScene;
	fmp::TagCloseUpConstPtr       d_tcu;
	QAction                     * d_newAntAction,*d_addIdentificationAction,*d_deletePoseAction;
	QAction                     * d_copyTimeAction;
	QToolBar                    * d_actionToolBar,*d_navigationToolBar;
	QToolBarSeparator           * d_toolBarSeparator;
	QDockWidget                 * d_tagExplorer;
};
