#pragma once

#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <fort/studio/MyrmidonTypes/Measurement.hpp>
#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>

#include <fort/studio/widget/vectorgraphics/Vector.hpp>
#include <fort/studio/widget/vectorgraphics/Capsule.hpp>

#include "Workspace.hpp"

namespace Ui {
class AntGeometryWorkspace;
}

class ExperimentBridge;
class QStandardItemModel;
class VectorialScene;

class AntGeometryWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit AntGeometryWorkspace(QWidget *parent = 0);
	virtual ~AntGeometryWorkspace();



	QAction * cloneAntShapeAction() const;


public slots:
	void nextCloseUp();
	void previousCloseUp();

private slots:
	void on_toolBox_currentChanged(int);

	void on_insertButton_clicked();
	void on_editButton_clicked();
	void on_comboBox_currentIndexChanged(int);

	void on_treeView_activated(const QModelIndex & index);

	void onAntSelected(bool);

	void onIdentificationAntPositionChanged(fmp::Identification::ConstPtr identification);
	void onIdentificationDeleted(fmp::IdentificationConstPtr ident);

	void onVectorUpdated();
	void onVectorCreated(QSharedPointer<Vector> vector);
	void onVectorRemoved(QSharedPointer<Vector> vector);

	void onCapsuleUpdated();
	void onCapsuleCreated(QSharedPointer<Capsule> capsule);
	void onCapsuleRemoved(QSharedPointer<Capsule> capsule);

	void onMeasurementModified(const fmp::MeasurementConstPtr &);
	void onMeasurementDeleted(QString tcuURI, quint32 mtID);

	void onCopyTime();

	void onCloneShapeActionTriggered();

protected:
	void changeEvent(QEvent * event) override;

	void initialize(ExperimentBridge * experiment) override;
	void setUp(QMainWindow * main, const NavigationAction & actions ) override;
	void tearDown(QMainWindow * main, const NavigationAction & actions ) override;

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

	void changeVectorType(Vector * vector,fmp::MeasurementTypeID mtID);
	void changeCapsuleType(Capsule * capsule,fmp::AntShapeTypeID stID);

	void onMeasurementModification(const QString & tcuURI,
	                               quint32 mtID,
	                               int direction);

	fmp::CapsulePtr capsuleFromScene(const QSharedPointer<Capsule> & capsule);
	void rebuildCapsules();

	fmp::MeasurementTypeConstPtr currentMeasurementType() const;

	fmp::AntShapeTypeConstPtr currentAntShapeType() const;

	int columnForMeasurementType(fmp::MeasurementTypeID mtID) const;

	void setColorFromType(quint32 typeID);

	void updateCloneAction();


	void setMeasurement(const QSharedPointer<Vector> & vector, fmp::MeasurementTypeID mtID);

	std::map<uint32_t,QSharedPointer<Vector>>::const_iterator findVector(Vector * vector) const;

	Ui::AntGeometryWorkspace    * d_ui;
	ExperimentBridge          * d_experiment;
	QStandardItemModel        * d_closeUps;
	fmp::TagCloseUp::ConstPtr   d_tcu;
	VectorialScene            * d_vectorialScene;
	Mode                        d_mode;

	std::map<uint32_t,QSharedPointer<Vector> > d_vectors;
	std::map<QSharedPointer<Capsule>,uint32_t> d_capsules;

	QAction * d_copyTimeAction;
	QAction * d_cloneShapeAction;
};
