#pragma once

#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>

#include "Workspace.hpp"

namespace Ui {
class AntGeometryWorkspace;
}

class ExperimentBridge;
class QStandardItemModel;
class Capsule;
class Vector;

class AntGeometryWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit AntGeometryWorkspace(QWidget * parent = nullptr);
	virtual ~AntGeometryWorkspace();

protected:
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;

	virtual quint32 typeFromComboBox() const = 0;

public slots:
	void setTagCloseUp(const fmp::TagCloseUp::ConstPtr & closeUp);

protected slots:
	virtual void on_insertButton_clicked() = 0;
	virtual void on_editButton_clicked() = 0;
	virtual void on_comboBox_currentIndexChanged(int) = 0;


private slots:
	void onIdentificationAntPositionChanged(const fmp::Identification::ConstPtr & identification);
	void onIdentificationDeleted(const fmp::Identification::ConstPtr & ident);

	void onCopyTime();

	void on_vectorialScene_modeChanged(VectorialScene::Mode mode);

protected:


	virtual void onClearScene() = 0;
	virtual void onNewCloseUp() = 0;


	void clearScene();
	void setColorFromType(quint32 typeID);


	QToolButton * d_editButton;
	QToolButton * d_insertButton;
	QComboBox   * d_comboBox;
	QToolBar    * d_editToolBar;
	QAction     * d_copyTimeAction;

	ExperimentBridge          * d_experiment;
	fmp::TagCloseUp::ConstPtr   d_closeUp;
	VectorialScene            * d_vectorialScene;
};

class AntMeasurementWorkspace : public AntGeometryWorkspace {
	Q_OBJECT
public:
	explicit AntMeasurmentWorkspace(QWidget * parent = nullptr);
	virtual ~AntMeasurementWorkspace();

protected :
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;

	quint32 typefromComboBox() const override;

	void onClearScene() override;
	void onNewCloseUp() override;


	void on_insertButton_clicked() override;
	void on_editButton_clicked() override;
	void on_comboBox_currentIndexChanged(int) override;



private slots:
	void onVectorUpdated();
	void onVectorCreated(QSharedPointer<Vector> vector);
	void onVectorRemoved(QSharedPointer<Vector> vector);


private:

	void setMeasurement(const QSharedPointer<Vector> & vector,
	                    fmp::MeasurementTypeID mtID);

	void changeVectorType(Vector * vector,fmp::MeasurementTypeID mtID);

	std::map<uint32_t,QSharedPointer<Vector>>::const_iterator
	findVector(Vector * vector) const;


	AntMeasurementListWidget * d_antCloseUps;
	MeasurementTypeWidget    * d_measurementTypes;
	QDockWidget              * d_closeUpDock, *d_shapeTypeDock;



	std::map<uint32_t,QSharedPointer<Vector> > d_vectors;
};

class AntShapeWorkspace : public AntGeometryWorkspace {
	Q_OBJECT
public:
	explicit AntShapeWorkspace(QWidget *parent = nullptr);
	virtual ~AntGeometryWorkspace();

	QAction * cloneAntShapeAction() const;

protected:
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;

	quint32 typefromComboBox() const override;

	void onClearScene() override;
	void onNewCloseUp() override;

	void on_insertButton_clicked() override;
	void on_editButton_clicked() override;
	void on_comboBox_currentIndexChanged(int) override;


private slots:
	void onCapsuleUpdated();
	void onCapsuleCreated(QSharedPointer<Capsule> capsule);
	void onCapsuleRemoved(QSharedPointer<Capsule> capsule);


	void onCloneShapeActionTriggered();
	void updateCloneAction();


	void changeCapsuleType(Capsule * capsule,fmp::AntShapeTypeID stID);

	fmp::CapsulePtr capsuleFromScene(const QSharedPointer<Capsule> & capsule);
	void rebuildCapsules();

	AntShapeListWidget * d_antCloseUps;
	AntShapeTypeWidget * d_shapeTypes;
	QDockWidget        * d_closeUpsDock, *d_shapeTypesDock;

	QAction * d_cloneShapeAction;

	std::map<QSharedPointer<Capsule>,uint32_t> d_capsules;
};


//OLD Implementation

class AntGeometryWorkspace : public Workspace {


protected:
	void changeEvent(QEvent * event) override;

	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions ) override;
	void tearDown(const NavigationAction & actions ) override;

private:


	enum class Mode {Shape,Measure};
	void setTagCloseUp(const fmp::TagCloseUp::ConstPtr & tcu);
	void setShappingMode();
	void setMeasureMode();

	void buildCloseUpList();
	void buildHeaders();

	void select(int increment);

	void clearScene();

	quint32 typeFromComboBox() const;



	fmp::CapsulePtr capsuleFromScene(const QSharedPointer<Capsule> & capsule);
	void rebuildCapsules();

	fmp::AntShapeTypeConstPtr currentAntShapeType() const;



	Ui::AntGeometryWorkspace    * d_ui;
	ExperimentBridge          * d_experiment;
	QStandardItemModel        * d_closeUps;
	fmp::TagCloseUp::ConstPtr   d_tcu;
	VectorialScene            * d_vectorialScene;
	Mode                        d_mode;

	std::map<uint32_t,QSharedPointer<Vector> > d_vectors;
};
