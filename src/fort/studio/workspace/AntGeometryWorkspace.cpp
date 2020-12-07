#include "AntGeometryWorkspace.hpp"
#include "ui_AntGeometryWorkspace.h"


#include <QStandardItemModel>
#include <QClipboard>
#include <QAction>
#include <QToolBar>
#include <QMainWindow>
#include <QDockWidget>
#include <QComboBox>


#include <fort/myrmidon/priv/Capsule.hpp>


#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/AntShapeBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>
#include <fort/studio/bridge/TagCloseUpBridge.hpp>

#include <fort/studio/Utils.hpp>
#include <fort/studio/Format.hpp>
#include <fort/studio/widget/vectorgraphics/Vector.hpp>
#include <fort/studio/widget/vectorgraphics/Capsule.hpp>
#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/CloneShapeDialog.hpp>
#include <fort/studio/widget/AntListWidget.hpp>
#include <fort/studio/widget/AntShapeTypeEditorWidget.hpp>
#include <fort/studio/widget/MeasurementTypeWidget.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>


AntGeometryWorkspace::AntGeometryWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_ui(new Ui::AntGeometryWorkspace)
	, d_experiment(nullptr)
	, d_copyTimeAction(nullptr)
	, d_vectorialScene( new VectorialScene(this)) {

	d_editToolBar = new QToolBar(this);

	d_editAction = d_editToolBar->addAction(QIcon::fromTheme("edit-select-symbolic"),
	                                        tr("Edit primitives"));
	d_editAction->setToolTip(tr("Edit primitives"));
	d_editAction->setStatusTip(d_editAction->toolTip());
	d_editAction->setObjectName("editAction");

	d_insertAction = d_editToolBar->addAction(QIcon::fromTheme("insert-object-symbolic"),
	                                          tr("Insert primitive"));
	d_insertAction->setToolTip(tr("Insert a new primitive"));
	d_insertAction->setStatusTip(d_insertAction->toolTip());
	d_insertAction->setObjectName("insertAction");
	d_comboBox = new QComboBox(this);
	d_comboBox->setObjectName("comboBox");

	d_editToolBar->addWidget(d_comboBox);

	d_insertAction->setCheckable(true);
	d_editAction->setCheckable(true);

	d_vectorialScene->setObjectName("vectorialScene");

	d_ui->setupUi(this);

	d_ui->vectorialView->setScene(d_vectorialScene);
	d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
	connect(d_ui->vectorialView,
	        &VectorialView::zoomed,
	        d_vectorialScene,
	        &VectorialScene::onZoomed);

	connect(d_vectorialScene, &VectorialScene::modeChanged,
	        this,&AntGeometryWorkspace::onVectorialSceneModeChanged);

}

AntGeometryWorkspace::~AntGeometryWorkspace() {
	delete d_ui;
}


void AntGeometryWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
	std::cerr << "Coucou geometry"<< std::endl;
	d_experiment = experiment;

	auto identifier = d_experiment->identifier();
	connect(identifier,
	        &IdentifierBridge::identificationAntPositionModified,
	        this,
	        &AntGeometryWorkspace::onIdentificationAntPositionChanged);
	connect(identifier,
	        &IdentifierBridge::identificationCreated,
	        this,
	        &AntGeometryWorkspace::onIdentificationAntPositionChanged);
	connect(identifier,
	        &IdentifierBridge::identificationDeleted,
	        this,
	        &AntGeometryWorkspace::onIdentificationDeleted);

	main->addToolBar(d_editToolBar);
	d_editToolBar->hide();

}

void AntGeometryWorkspace::setUp(const NavigationAction & actions ) {
	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&AntGeometryWorkspace::onCopyTime);

	actions.NavigationToolBar->show();
	d_copyTimeAction = actions.CopyCurrentTime;
	d_editToolBar->show();
}

void AntGeometryWorkspace::tearDown(const NavigationAction & actions ) {
	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&AntGeometryWorkspace::onCopyTime);


	d_copyTimeAction = nullptr;
	actions.NavigationToolBar->hide();
	d_editToolBar->hide();
}


void AntGeometryWorkspace::onIdentificationAntPositionChanged(const fmp::Identification::ConstPtr & identification) {
	if ( d_closeUp == nullptr
	     || identification->TagValue() != d_closeUp->TagValue()
	     || identification->IsValid(d_closeUp->Frame().Time()) == false ) {
		return;
	}

	Eigen::Vector2d position;
	double angle;

	identification->ComputePositionFromTag(position,
	                                       angle,d_closeUp->TagPosition(),
	                                       d_closeUp->TagAngle());

	d_vectorialScene->setPoseIndicator(QPointF(position.x(),
	                                           position.y()),
	                                   angle);
}

void AntGeometryWorkspace::onIdentificationDeleted(const fmp::Identification::ConstPtr & identification) {
	if ( d_closeUp == nullptr
	     || d_closeUp->TagValue() != identification->TagValue()
	     || identification->IsValid(d_closeUp->Frame().Time()) == false ) {
		return;
	}
	setTagCloseUp(nullptr);
}

void AntGeometryWorkspace::onCopyTime() {
	if ( d_closeUp == nullptr ) {
		return;
	}

	QApplication::clipboard()->setText(ToQString(d_closeUp->Frame().Time()));
}

void AntGeometryWorkspace::clearScene() {
	onClearScene();
	d_vectorialScene->setBackgroundPicture("");
	d_vectorialScene->clearPoseIndicator();
	d_vectorialScene->clearStaticPolygon();
	d_ui->vectorialView->setBannerMessage("",QColor());
}

void AntGeometryWorkspace::setTagCloseUp(const fmp::TagCloseUp::ConstPtr & closeUp) {
	if ( d_closeUp == closeUp ) {
		return;
	}
	d_closeUp = closeUp;
	clearScene();
	if ( d_copyTimeAction != nullptr ) {
		d_copyTimeAction->setEnabled(d_closeUp != nullptr);
	}

	if ( d_closeUp == nullptr ) {
		return;
	}

	d_vectorialScene->setBackgroundPicture(d_closeUp->AbsoluteFilePath().c_str());
	const auto & tagPosition = d_closeUp->TagPosition();
	d_ui->vectorialView->centerOn(QPointF(tagPosition.x(),tagPosition.y()));
	d_vectorialScene->setStaticPolygon(d_closeUp->Corners(),QColor(255,0,0));

	auto identification = d_experiment->identifier()->identify(d_closeUp->TagValue(),
	                                                           d_closeUp->Frame().Time());

	if ( identification != nullptr ) {
		onIdentificationAntPositionChanged(identification);
	}

	onNewCloseUp();
}

void AntGeometryWorkspace::onVectorialSceneModeChanged(VectorialScene::Mode mode) {
	d_editAction->setChecked(mode == VectorialScene::Mode::Edit);
	d_insertAction->setChecked(mode != VectorialScene::Mode::Edit);
}

void AntGeometryWorkspace::setColorFromType(quint32 typeID) {
	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(typeID)));
}


AntMeasurementWorkspace::AntMeasurementWorkspace(QWidget * parent)
	: AntGeometryWorkspace(parent) {

	d_editAction->setText(tr("Edit Measurement Tool"));
	d_editAction->setToolTip(tr("Move and delete measurements with the mouse"));
	d_editAction->setStatusTip(d_editAction->toolTip());

	d_insertAction->setText(tr("New Measurement Tool"));
	d_insertAction->setToolTip(tr("Add a new  measurement by clicking two points on the image"));
	d_insertAction->setStatusTip(d_insertAction->toolTip());

	d_antCloseUps = new AntMeasurementListWidget(this);
	d_antCloseUps->setObjectName("antCloseUps");

	d_measurementTypes = new MeasurementTypeWidget(this);
	d_measurementTypes->setObjectName("measurementTypes");

	d_closeUpsDock = new QDockWidget(tr("Ant Close-Ups"),this);
	d_closeUpsDock->setWidget(d_antCloseUps);
	connect(d_antCloseUps, &AntMeasurementListWidget::currentCloseUpChanged,
	        this,&AntGeometryWorkspace::setTagCloseUp);

	d_measurementTypesDock = new QDockWidget(tr("Measurement Types"),this);
	d_measurementTypesDock->setWidget(d_measurementTypes);

	connect(d_vectorialScene,
	        &VectorialScene::vectorCreated,
	        this,
	        &AntMeasurementWorkspace::onVectorCreated);

	connect(d_vectorialScene,
	        &VectorialScene::vectorRemoved,
	        this,
	        &AntMeasurementWorkspace::onVectorRemoved);

}

AntMeasurementWorkspace::~AntMeasurementWorkspace() {
}


void AntMeasurementWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
	std::cerr << "Coucou measurement"<< std::endl;
	AntGeometryWorkspace::initialize(main,experiment);

	d_measurementTypes->setup(experiment->measurements());
	d_antCloseUps->initialize(experiment);

	d_comboBox->setModel(experiment->measurements()->typeModel());

	main->addDockWidget(Qt::LeftDockWidgetArea,d_closeUpsDock);
	d_closeUpsDock->hide();
	main->addDockWidget(Qt::LeftDockWidgetArea,d_measurementTypesDock);
	d_measurementTypesDock->hide();
}

void AntMeasurementWorkspace::setUp(const NavigationAction & actions ) {
	AntGeometryWorkspace::setUp(actions);
	connect(actions.NextTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextAnt);
	connect(actions.PreviousTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousAnt);

	connect(actions.NextCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextAntCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousAntCloseUp);

	actions.NextTag->setEnabled(true);
	actions.PreviousTag->setEnabled(true);
	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);

	d_closeUpsDock->show();
	d_measurementTypesDock->show();
}

void AntMeasurementWorkspace::tearDown(const NavigationAction & actions ) {
	AntGeometryWorkspace::tearDown(actions);

	disconnect(actions.NextTag,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::nextAnt);
	disconnect(actions.PreviousTag,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousAnt);

	disconnect(actions.NextCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::nextAntCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousAntCloseUp);

	actions.NextTag->setEnabled(false);
	actions.PreviousTag->setEnabled(false);
	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);

	d_closeUpsDock->hide();
	d_measurementTypesDock->hide();
}

void AntMeasurementWorkspace::onClearScene() {
	QSignalBlocker blocker(d_vectorialScene);
	d_vectorialScene->clearVectors();
	d_vectors.clear();
}

void AntMeasurementWorkspace::onNewCloseUp() {
	d_vectors.clear();
	if ( d_closeUp == nullptr ) {
		return;
	}

	for ( size_t i = 0; i < d_comboBox->count(); ++i) {
		auto mType = d_comboBox->itemData(i,Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
		if ( mType == nullptr ) {
			continue;
		}
		auto m = d_experiment->measurements()->measurementForCloseUp(d_closeUp->URI(),
		                                                             mType->MTID());
		if ( m == nullptr ) {
			continue;
		}

		fmp::Isometry2Dd tagToOrig(d_closeUp->TagAngle(),d_closeUp->TagPosition());
		Eigen::Vector2d start = tagToOrig * m->StartFromTag();
		Eigen::Vector2d end = tagToOrig * m->EndFromTag();

		setColorFromType(mType->MTID());
		auto vector = d_vectorialScene->appendVector(QPointF(start.x(),
		                                                     start.y()),
		                                             QPointF(end.x(),
		                                                     end.y()));

		d_vectors.insert(std::make_pair(mType->MTID(),vector));

		connect(vector.data(),
		        &Shape::updated,
		        this,
		        &AntMeasurementWorkspace::onVectorUpdated);

	}

	setColorFromType(typeFromComboBox());
}

quint32 AntMeasurementWorkspace::typeFromComboBox() const {
	auto mType = d_comboBox->currentData(Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
	if ( mType == nullptr ) {
		return 0;
	}
	return mType->MTID();
}

void AntMeasurementWorkspace::on_insertAction_triggered() {
	d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
}

void AntMeasurementWorkspace::on_editAction_triggered() {
	d_vectorialScene->setMode(VectorialScene::Mode::Edit);
}

void AntMeasurementWorkspace::on_comboBox_currentIndexChanged(int) {
	quint32 type = typeFromComboBox();

	setColorFromType(type);

	if ( d_closeUp == nullptr ) {
		return;
	}

	for ( const auto & item : d_vectorialScene->selectedItems() ) {
		auto v = dynamic_cast<Vector*>(item);
		if ( v == nullptr ) {
			continue;
		}
		changeVectorType(v,type);
	}
}

void AntMeasurementWorkspace::onVectorUpdated() {
	if ( d_closeUp == nullptr ) {
		return;
	}

	auto sender = QObject::sender();
	auto fi = findVector(dynamic_cast<Vector*>(sender));
	if ( fi == d_vectors.end() ) {
		qDebug() << "[AntMeasurementWorkspace]: could not find back sender";
		return;
	}
	setMeasurement(fi->second,fi->first);
}

void AntMeasurementWorkspace::onVectorCreated(QSharedPointer<Vector> vector) {\
	if ( d_closeUp == nullptr ) {
		qDebug() << "[AntGeometryWorkspace]: Vector created without tcu";
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}
	auto mtID = typeFromComboBox();
	if ( mtID == 0 ) {
		qDebug() << "No measurement type selected";
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	if ( d_vectors.count(mtID)  != 0 ) {
		qWarning() << "Measurement already exist in Close-Up for type " << mtID;
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	if ( d_experiment->measurements()->setMeasurement(d_closeUp,
	                                                  mtID,
	                                                  vector->startPos(),
	                                                  vector->endPos()) == false ) {
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	connect(vector.data(),&Shape::updated,
	        this,&AntMeasurementWorkspace::onVectorUpdated);

}

void AntMeasurementWorkspace::onVectorRemoved(QSharedPointer<Vector> vector) {
	if ( d_closeUp == nullptr ) {
		return;
	}
	auto fi = findVector(vector.data());
	if ( fi == d_vectors.end() ) {
		qDebug() << "[AntGeometryWorkspace]: could not find back vector";
		return;
	}
	auto m = d_experiment->measurements()->measurementForCloseUp(d_closeUp->URI(),fi->first);
	if ( !m ) {
		qWarning() << "No measurement of type " << fi->first << " for " << d_closeUp->URI().c_str();
		return;
	}
	d_vectors.erase(fi);
	d_experiment->measurements()->deleteMeasurement(m);

}

void AntMeasurementWorkspace::setMeasurement(const QSharedPointer<Vector> & vector, fmp::MeasurementTypeID mtID) {
	if ( d_experiment == nullptr || d_closeUp == nullptr ) {
		return;
	}

	d_experiment->measurements()->setMeasurement(d_closeUp,
	                                             mtID,
	                                             vector->startPos(),
	                                             vector->endPos());

	auto m = d_experiment->measurements()->measurementForCloseUp(d_closeUp->URI(),mtID);
	if ( m == nullptr ) {
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	fmp::Isometry2Dd tagToOrig(d_closeUp->TagAngle(),
	                           d_closeUp->TagPosition());
	Eigen::Vector2d start = tagToOrig * m->StartFromTag();
	Eigen::Vector2d end = tagToOrig * m->EndFromTag();
	vector->setStartPos(QPointF(start.x(),start.y()));
	vector->setEndPos(QPointF(end.x(),end.y()));
}

std::map<uint32_t,QSharedPointer<Vector>>::const_iterator
AntMeasurementWorkspace::findVector(Vector * vector) const {
	return std::find_if(d_vectors.begin(),
	                    d_vectors.end(),
	                    [vector](const std::pair<uint32_t,QSharedPointer<Vector>> & item ) {
		                    return item.second.data() == vector;
	                    });
}

void AntMeasurementWorkspace::changeVectorType(Vector * vector,
                                               fmp::MeasurementTypeID mtID) {
	auto fi = findVector(vector);
	if ( d_closeUp == nullptr
	     || d_vectors.count(mtID) != 0
	     || fi == d_vectors.end() ) {
		return;
	}

	auto m = d_experiment->measurements()->measurementForCloseUp(d_closeUp->URI(),
	                                                             fi->first);
	if ( m != nullptr ) {
		d_experiment->measurements()->deleteMeasurement(m);
	}

	d_vectors.insert(std::make_pair(mtID,fi->second));
	fi->second->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(mtID)));
	setMeasurement(fi->second,mtID);
	d_vectorialScene->update();
	d_vectors.erase(fi);
}


AntShapeWorkspace::AntShapeWorkspace(QWidget *parent)
	:  AntGeometryWorkspace(parent) {

	d_editAction->setText(tr("Edit Capsule Tool"));
	d_editAction->setToolTip(tr("Move and delete capsules with the mouse"));
	d_editAction->setStatusTip(d_editAction->toolTip());

	d_insertAction->setText(tr("New Capsule Tool"));
	d_insertAction->setToolTip(tr("Add a new capsule by clicking two points on the image"));
	d_insertAction->setStatusTip(d_insertAction->toolTip());


	d_antCloseUps = new AntShapeListWidget(this);
	d_antCloseUps->setObjectName("antCloseUps");

	d_shapeTypes = new AntShapeTypeEditorWidget(this);
	d_shapeTypes->setObjectName("shapeTypes");

	d_closeUpsDock = new QDockWidget(tr("Ant Close-Ups"),this);
	d_closeUpsDock->setWidget(d_antCloseUps);
	connect(d_antCloseUps, &AntShapeListWidget::currentCloseUpChanged,
	        this,&AntGeometryWorkspace::setTagCloseUp);

	d_shapeTypesDock = new QDockWidget(tr("Ant Shape Types"),this);
	d_shapeTypesDock->setWidget(d_shapeTypes);

	connect(d_vectorialScene,
	        &VectorialScene::capsuleCreated,
	        this,
	        &AntShapeWorkspace::onCapsuleCreated);

	connect(d_vectorialScene,
	        &VectorialScene::capsuleRemoved,
	        this,
	        &AntShapeWorkspace::onCapsuleRemoved);

	d_cloneShapeAction = new QAction(tr("Clone current Ant Shape"),this);
	d_cloneShapeAction->setObjectName("cloneShapeAction");
	d_cloneShapeAction->setToolTip(tr("Clone current shape to other ants"));
    d_cloneShapeAction->setEnabled(false);
    connect(d_cloneShapeAction,&QAction::triggered,
            this,&AntShapeWorkspace::onCloneShapeActionTriggered);
}

AntShapeWorkspace::~AntShapeWorkspace() {

}

QAction * AntShapeWorkspace::cloneAntShapeAction() const {
	return d_cloneShapeAction;
}


void AntShapeWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
	AntGeometryWorkspace::initialize(main,experiment);
	d_shapeTypes->setup(experiment->antShapeTypes());
	d_antCloseUps->initialize(experiment);

	d_comboBox->setModel(experiment->antShapeTypes()->shapeModel());

	updateCloneAction();
	main->addDockWidget(Qt::LeftDockWidgetArea,d_closeUpsDock);
	d_closeUpsDock->hide();
	main->addDockWidget(Qt::LeftDockWidgetArea,d_shapeTypesDock);
	d_shapeTypesDock->hide();

}

void AntShapeWorkspace::setUp(const NavigationAction & actions ) {
	AntGeometryWorkspace::setUp(actions);
	connect(actions.NextTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextAnt);
	connect(actions.PreviousTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousAnt);

	connect(actions.NextCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextAntCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousAntCloseUp);

	actions.NextTag->setEnabled(true);
	actions.PreviousTag->setEnabled(true);
	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);

	d_closeUpsDock->show();
	d_shapeTypesDock->show();

}

void AntShapeWorkspace::tearDown(const NavigationAction & actions ) {
	AntGeometryWorkspace::tearDown(actions);

	disconnect(actions.NextTag,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::nextAnt);
	disconnect(actions.PreviousTag,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousAnt);

	disconnect(actions.NextCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::nextAntCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousAntCloseUp);

	actions.NextTag->setEnabled(false);
	actions.PreviousTag->setEnabled(false);
	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);

	d_closeUpsDock->hide();
	d_shapeTypesDock->hide();
}

quint32 AntShapeWorkspace::typeFromComboBox() const {
	auto shapeType = d_comboBox->currentData(Qt::UserRole+1).value<fmp::AntShapeType::Ptr>();
	if ( shapeType == nullptr ) {
		return 0;
	}
	return shapeType->TypeID();
}

void AntShapeWorkspace::onClearScene() {
	QSignalBlocker blocker(d_vectorialScene);
	d_vectorialScene->clearCapsules();
	d_capsules.clear();
}

void AntShapeWorkspace::onNewCloseUp() {
	if ( d_closeUp == nullptr
	     || d_experiment == nullptr ) {
		return;
	}
	auto identification = d_experiment->identifier()->identify(d_closeUp->TagValue(),
	                                                           d_closeUp->Frame().Time());

	if ( identification == nullptr ) {
		return;
	}
	auto ant = identification->Target();


	fmp::Isometry2Dd tagToOrig(d_closeUp->TagAngle(),d_closeUp->TagPosition());
	auto antToOrig = tagToOrig * identification->AntToTagTransform();
	for ( const auto & [stID,c] : ant->Capsules() ) {
		Eigen::Vector2d c1 = antToOrig * c.C1();
		Eigen::Vector2d c2 = antToOrig * c.C2();
		setColorFromType(stID);
		auto capsule = d_vectorialScene->appendCapsule(QPointF(c1.x(),c1.y()),
		                                               QPointF(c2.x(),c2.y()),
		                                               c.R1(),
		                                               c.R2());
		d_capsules.insert(std::make_pair(capsule,stID));
		connect(capsule.data(),
		        &Shape::updated,
		        this,
		        &AntShapeWorkspace::onCapsuleUpdated);
	}

	setColorFromType(typeFromComboBox());
}

void AntShapeWorkspace::on_insertAction_triggered() {
	d_vectorialScene->setMode(VectorialScene::Mode::InsertCapsule);
}

void AntShapeWorkspace::on_editAction_triggered() {
	d_vectorialScene->setMode(VectorialScene::Mode::Edit);
}

void AntShapeWorkspace::on_comboBox_currentIndexChanged(int) {
	quint32 type = typeFromComboBox();
	setColorFromType(type);

	if ( d_closeUp == nullptr ) {
		return;
	}

	for ( const auto & item : d_vectorialScene->selectedItems() ) {
		auto v = dynamic_cast<Capsule*>(item);
		if ( v == nullptr ) {
			continue;
		}
		changeCapsuleType(v,type);
	}
}

void AntShapeWorkspace::onCapsuleUpdated() {
	rebuildCapsules();
}

void AntShapeWorkspace::onCapsuleCreated(QSharedPointer<Capsule> capsule) {
	auto antID  = selectedAntID();
	if ( antID == 0
	     || d_experiment == nullptr ) {
		qDebug() << "[AntShapeWorkspace]: Capsule created without an ant";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	}
	quint32 type = typeFromComboBox();
	if ( type == 0 ) {
		qDebug() << "[AntShapeWorkspace]: no type selected";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	}

	auto c = capsuleFromScene(capsule);
	if ( c == nullptr )  {
		qDebug() << "[AntShapeWorkspace]: could not compute capsule, removing it";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	}

	if ( d_experiment->antShapes()->addCapsule(antID,
	                                           type,
	                                           c) == -1 ) {
		qDebug() << "[AntShapeWorkspace]: could not add capsule, removing it from the scene";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
	}


	d_capsules.insert(std::make_pair(capsule,type));

	connect(capsule.data(),
	        &Shape::updated,
	        this,
	        &AntShapeWorkspace::onCapsuleUpdated);

}

void AntShapeWorkspace::onCapsuleRemoved(QSharedPointer<Capsule> capsule) {
	auto fi = d_capsules.find(capsule);
	if ( fi == d_capsules.end() ) {
		qDebug() << "[AntEditorWidget]: Could not find back capsule";
		return;
	}

	d_capsules.erase(fi);
	rebuildCapsules();
}

void AntShapeWorkspace::onCloneShapeActionTriggered() {
	auto antID = selectedAntID();

	if ( d_experiment == nullptr
	     || antID == 0 ) {
		return;
	}

	auto opts = CloneShapeDialog::get(d_experiment,
	                                  this);

	if ( !opts == true ) {
		qWarning() << "Not cloning ants";
		return;
	}

	d_experiment->antShapes()->cloneShape(antID,opts->ScaleToSize,opts->OverwriteShapes);
}

void AntShapeWorkspace::updateCloneAction() {
	auto antID = selectedAntID();
	if ( d_experiment == nullptr
	     || d_closeUp == nullptr
	     || antID == 0 ) {
		d_cloneShapeAction->setEnabled(false);
		return;
	}
	const auto & capsules = d_experiment->antShapes()->capsuleForAntID(antID);
	d_cloneShapeAction->setEnabled(capsules.empty() == false);
}


void AntShapeWorkspace::changeCapsuleType(Capsule * capsule,fmp::AntShapeTypeID shapeTypeID) {
	auto fi = std::find_if(d_capsules.begin(),
	                       d_capsules.end(),
	                       [capsule](const std::pair<QSharedPointer<Capsule>,uint32_t> & iter) -> bool {
		                       return iter.first.data() == capsule;
	                       });

	auto antID = selectedAntID();

	if ( d_closeUp == nullptr
	     || d_experiment == nullptr
	     || antID == 0
	     || fi == d_capsules.end() ) {
		return;
	}

	fi->second = shapeTypeID;
	fi->first->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(shapeTypeID)));
	d_vectorialScene->update();
	rebuildCapsules();
}

fmp::CapsulePtr AntShapeWorkspace::capsuleFromScene(const QSharedPointer<Capsule> & capsule) {
	if ( d_closeUp == nullptr
	     || d_experiment == nullptr) {
		return fmp::CapsulePtr();
	}
	auto identification = d_experiment->identifier()->identify(d_closeUp->TagValue(),
	                                                  d_closeUp->Frame().Time());
	if ( identification == nullptr ) {
		qDebug() << "[AntShapeWorkspace]: No Identification for " << d_closeUp->URI().c_str();
		return fmp::CapsulePtr();
	}

	// we need origToAnt
	// we have origToTag and antToTag
	auto origToAnt = identification->AntToTagTransform().inverse() * d_closeUp->ImageToTag();
	Eigen::Vector2d c1 = origToAnt * ToEigen(capsule->c1Pos());
	Eigen::Vector2d c2 = origToAnt * ToEigen(capsule->c2Pos());

	return std::make_shared<fmp::Capsule>(c1,c2,capsule->r1(),capsule->r2());
}

void AntShapeWorkspace::rebuildCapsules() {
	auto antID = selectedAntID();
	if ( antID == 0
	     || d_experiment == nullptr ) {
		return;
	}
	d_experiment->antShapes()->clearCapsule(antID);
	for ( const auto & [sceneCapsule,shapeTypeID] : d_capsules ) {
		auto capsule = capsuleFromScene(sceneCapsule);
		if ( capsule == nullptr ) {
			continue;
		}
		d_experiment->antShapes()->addCapsule(antID,shapeTypeID,capsule);
	}
}

quint32 AntShapeWorkspace::selectedAntID() const {
	if ( d_antCloseUps == nullptr ) {
		return 0;
	}
	return d_antCloseUps->selectedAntID();
}
