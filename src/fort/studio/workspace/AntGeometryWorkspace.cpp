#include "AntGeometryWorkspace.hpp"
#include "ui_AntGeometryWorkspace.h"


#include <QStandardItemModel>
#include <QClipboard>
#include <QAction>

#include <fort/myrmidon/priv/Capsule.hpp>


#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/SelectedAntBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>
#include <fort/studio/bridge/TagCloseUpBridge.hpp>


#include <fort/studio/Format.hpp>
#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/CloneShapeDialog.hpp>
#include <fort/studio/Utils.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>


AntGeometryWorkspace::AntGeometryWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_ui(new Ui::AntGeometryWorkspace)
	, d_experiment(nullptr)
	, d_copyTimeAction(nullptr)
	, d_vectorialScene( new VectorialScene(this)) {

	d_toolBar = new QToolBar(this);

	d_editButton = d_toolBar->addAction(QIcon::fromTheme("edit-select-symbolic"));
	d_editButton->setToolTip(tr("Edit primitives"));
	d_editButton->setStatusTip(d_editButton->toolTip());
	d_editButton->setObjectName("editButton");

	d_insertButton = d_toolBar->addAction(QIcon::fromTheme("insert-object-symbolic"));
	d_insertButton->setToolTip(tr("Insert a new primitive"));
	d_insertButton->setStatusTip(d_insertButton->toolTip());
	d_insertButton->setObjectName("insertButton");
	d_comboBox = new QComboBox(this);
	d_comboBox->setObjectName("comboBox");

	d_toolBar->addWidget(d_comboBox);

	d_insertButton->setCheckable(true);
	d_editButton->setCheckable(true);

	d_vectorialScene->setObjectName("vectorialScene");

	d_ui->setupUi(this);

	d_ui->vectorialView->setScene(d_vectorialScene);
	d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
	connect(d_ui->vectorialView,
	        &VectorialView::zoomed,
	        d_vectorialScene,
	        &VectorialScene::onZoomed);

}

AntGeometryWorkspace::~AntGeometryWorkspace() {
	delete d_ui;
}


void AntGeometryWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
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

	d_copyTimeAction = actions.CopyCurrentTime;
	d_editToolBar->show();
}

void AntGeometryWorkspace::tearDown(const NavigationAction & actions ) {
	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&AntGeometryWorkspace::onCopyTime);

	d_copyTimeAction = nullptr;
	d_editToolBar->hide();
}


void AntGeometryWorkspace::onIdentificationAntPositionChanged(const fmp::Identification::ConstPtr & identification) {
	if ( d_closeUp
	     || identification->TagValue() != d_closeUp->TagValue()
	     || identification->IsValid(d_closeUp->Frame().Time()) == false ) {
		return;
	}
	Eigen::Vector2d position;
	double angle;
	identification->ComputePositionFromTag(position,angle,d_closeUp->TagPosition(),d_tcu->TagAngle());
	d_vectorialScene->setPoseIndicator(QPointF(position.x(),
	                                           position.y()),
	                                   angle);
}

void AntGeometryWorkspace::onIdentificationDeleted(const fmp::Identification::ConstPtr & identification) {
	if ( !d_closeUp
	     || d_closeUp->TagValue() != identification->TagValue()
	     || identification->IsValid(d_closeUp->Frame().Time()) == false ) {
		return;
	}
	onTagCloseUp(nullptr);
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
	d_vectorialScene->clearPositionIndicator();
	d_vectorialScene->clearStaticPolygon();
	d_ui->vectorialView->setBannerMessage("",QColor());
}

void AntGeometryWorkspace::onTagCloseUp(const fmp::TagCloseUp::ConstPtr & closeUp) {
	if ( d_closeUp== closeUp ) {
		return;
	}
	d_closeUp = closeUp;
	clearScene();
	if ( d_copyTimeAction != nullptr ) {
		d_copyTimeAction->setEnabled(d_closeUp != nullptr);
	}

	if ( !d_closeUp ) {
		return;
	}

	d_vectorialScene->setBackgroundPicture(d_closeUp->AbsoluteFilePath().c_str());
	const auto & tagPosition = d_closeUp->TagPosition();
	d_ui->vectorialView->centerOn(QPointF(tagPosition.x(),tagPosition.y()));
	d_vectorialScene->setStaticPolygon(d_closeUp->Corners(),QColor(255,0,0));
	auto identification = d_experiment->identifier()->identify(d_closeUp->TagValue(),tcu->Frame().Time());
	if ( identification != nullptr ) {
		onIdentificationAntPositionChanged(identification);
	}

	onNewCloseUp();
}

void AntGeometryWorkspace::on_vectorialScene_modeChanged(VectorialScene::Mode mode) {
	d_editButton->setChecked(mode == VectorialScene::Mode::Edit);
	d_insertButton->setChecked(mode != VectorialScene::Mode::Edit);
}

void AntGeometryWorkspace::setColorFromType(quint32 typeID) {
	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(typeID)));
}


AntMeasurementWorkspace::AntMeasurementWorkspace(QWidget * parent)
	: AntGeometryWorkspace(parent) {

	d_antCloseUps = new AntMeasurementListWidget(this);
	d_antCloseUps->setObjectName("antCloseUps");

	d_measurementTypes = new MeasurementTypeWidget(this);
	d_measurementTypes->setObjectName("measurementTypes");

	d_closeUpDock = new QDockWidget(tr("Ant Close-Ups"),this);
	d_closeUpDock->setWidget(d_antCloseUps);
	connect(d_antCloseUps, &AntMeasurementListWidget::currentCloseUpChanged,
	        this,&AntGeometryWorkspace::setTagCloseUp);

	d_measurementTypesDock = new QDockWidget(tr("Measurement Types"),this);
	d_measurementTypesDock->setWidget(d_measurementTypes);

	connect(d_vectorialScene,
	        &VectorialScene::vectorCreated,
	        this,
	        &AntGeometryWorkspace::onVectorCreated);

	connect(d_vectorialScene,
	        &VectorialScene::vectorRemoved,
	        this,
	        &AntGeometryWorkspace::onVectorRemoved);

}

AntMeasurementWorkspace::~AntMeasurementWorkspace() {
}


void AntMeasurementWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
	AntGeometryWorkspace::initialize(main,experiment);

	d_measurementTypes->setup(experiment->measurements());
	d_antCloseUps->initialize(experiment);

	auto measurements = experiment->measurements();
	connect(measurements,
	        &MeasurementBridge::measurementModified,
	        this,
	        &AntGeometryWorkspace::onMeasurementModified);
	connect(measurements,
	        &MeasurementBridge::measurementCreated,
	        this,
	        &AntGeometryWorkspace::onMeasurementModified);
	connect(measurements,
	        &MeasurementBridge::measurementDeleted,
	        this,
	        &AntGeometryWorkspace::onMeasurementDeleted);

	d_comboBox->setModel(experiment->measurements()->typeModel());

	main->addDockWidget(Qt::LeftDockWidgetArea,d_closeUpsDock);
	d_closeUpDocks->hide();
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
	        d_antCloseUps,&AntCloseUpExplorer::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousCloseUp);

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
	           d_antCloseUps,&AntCloseUpExplorer::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousCloseUp);

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

	for ( size_t i = 0; i < d_ui->comboBox->count(); ++i) {
		auto mType = d_ui->comboBox->itemData(i,Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
		if ( !mType ) {
			continue;
		}
		auto m = d_experiment->measurements()->measurementForCloseUp(d_closeUp->URI(),
		                                                             mType->MTID());
		if ( !m ) {
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
		        &AntGeometryWorkspace::onVectorUpdated);

	}

	setColorFromType(typeFromComboBox());
}

quint32 AntMeasurementWorkspace::typeFromComboBox() const {
	auto mType = d_ui->comboBox->currentData(Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
	if ( mType == nullptr ) {
		return 0;
	}
	return mType->MTID();
}

void AntMeasurementWorkspace::on_insertButton_clicked() {
	d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
}

void AntMeasurementWorkspace::on_editButton_clicked() {
	d_vectorialScene->setMode(VectorialScene::Mode::Edit);
}

void AntMeasurementWorkspace::on_comboBox_currentIndexChanged(int) {
	quint32 type = typeFromComboBox();

	setColorFromType(type);

	if ( d_closeUp == nullptr ) {
		return;
	}

	for ( const auto : d_vectorialScene->selectedItems() ) {
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
		qWarning() << "Measurement already exist in TCU for " << ToQString(measurementType->Name());
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	if ( d_experiment->measurements()->setMeasurement(d_tcu,
	                                                  mtID,
	                                                  vector->startPos(),
	                                                  vector->endPos()) == false ) {
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		return;
	}

	connect(vector.data(),&Shape::updated,
	        this,&AntGeometryWorkspace::onVectorUpdated);

}

void AntMeasurementWorkspace::onVectorRemoved(QSharedPointer<Vector> vector) {
	if ( !d_tcu ) {
		return;
	}
	auto fi = findVector(vector.data());
	if ( fi == d_vectors.end() ) {
		qDebug() << "[AntGeometryWorkspace]: could not find back vector";
		return;
	}
	auto m = d_experiment->measurements()->measurementForCloseUp(d_tcu->URI(),fi->first);
	if ( !m ) {
		qWarning() << "No measurement of type " << fi->first << " for " << ToQString(d_tcu->URI());
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

	auto m = d_experiment->measurements()->measurementForCloseUp(d_tcu->URI(),mtID);
	if ( !m ) {
		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
	} else {
		fmp::Isometry2Dd tagToOrig(d_tcu->TagAngle(),d_tcu->TagPosition());
		Eigen::Vector2d start = tagToOrig * m->StartFromTag();
		Eigen::Vector2d end = tagToOrig * m->EndFromTag();
		vector->setStartPos(QPointF(start.x(),start.y()));
		vector->setEndPos(QPointF(end.x(),end.y()));
	}
}

std::map<uint32_t,QSharedPointer<Vector>>::const_iterator
AntMEasurementWorkspace::findVector(Vector * vector) const {
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

	auto m = d_experiment->measurements()->measurementForCloseUp(d_tcu->URI(),fi->first);
	if ( m ) {
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

	d_antCloseUps = new AntShapeListWidget(this);
	d_antCloseUps->setObjectName("antCloseUps");

	d_shapeTypes = new AntShapeTypeWidget(this);
	d_shapeTypes->setObjectName("shapeTypes");

	d_closeUpsDock = new QDockWidget(tr("Ant Close-Ups"),this);
	d_closeUpsDock->setWidget(d_antCloseUps);
	connect(d_antCloseUps, &AnTShapeListWidget::currentCloseUpChanged,
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

AntShapeWorkspace::~AntGeometryWorkspace() {

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
}

void AntShapeWorkspace::setUp(const NavigationAction & actions ) {
	AntGeometryWorkspace::setUp(actions);
	connect(actions.NextTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextAnt);
	connect(actions.PreviousTag,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousAnt);

	connect(actions.NextCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        d_antCloseUps,&AntCloseUpExplorer::previousCloseUp);

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
	           d_antCloseUps,&AntCloseUpExplorer::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           d_antCloseUps,&AntCloseUpExplorer::previousCloseUp);

	actions.NextTag->setEnabled(false);
	actions.PreviousTag->setEnabled(false);
	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);

	d_closeUpsDock->hide();
	d_shapeTypesDock->hide();
}

quint32 AntShapeWorkspace::typefromComboBox() const {

}

void AntShapeWorkspace::onClearScene() {

}

void AntShapeWorkspace::onNewCloseUp() {

}

void AntShapeWorkspace::on_insertButton_clicked() {

}

void AntShapeWorkspace::on_editButton_clicked() {

}

void AntShapeWorkspace::on_comboBox_currentIndexChanged(int) {

}

void AntShapeWorkspace::onCapsuleUpdated() {

}

void AntShapeWorkspace::onCapsuleCreated(QSharedPointer<Capsule> capsule) {

}

void AntShapeWorkspace::onCapsuleRemoved(QSharedPointer<Capsule> capsule) {

}

void AntShapeWorkspace::onCloneShapeActionTriggered() {

}

void AntShapeWorkspace::updateCloneAction() {

}


void AntShapeWorkspace::changeCapsuleType(Capsule * capsule,fmp::AntShapeTypeID stID) {

}

fmp::CapsulePtr AntShapeWorkspace::capsuleFromScene(const QSharedPointer<Capsule> & capsule) {

}

void AntShapeWorkspace::rebuildCapsules() {

}

/// OLD IMPLEMENTATION BEGIN



AntGeometryWorkspace::AntGeometryWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_ui(new Ui::AntGeometryWorkspace)
	, d_experiment(nullptr)
	, d_closeUps(new QStandardItemModel(this) )
	, d_vectorialScene( new VectorialScene(this) )
	, d_copyTimeAction(nullptr)
	, d_cloneShapeAction( new QAction(tr("Clone Current Ant Shape"),this) ){
	d_ui->setupUi(this);
	d_ui->treeView->setModel(d_closeUps);


	auto hHeader = d_ui->treeView->header();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);



}

AntGeometryWorkspace::~AntGeometryWorkspace() {
	delete d_ui;
}

void AntGeometryWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
}


void AntGeometryWorkspace::setShappingMode() {
	on_comboBox_currentIndexChanged(d_ui->comboBox->currentIndex());
	d_vectorialScene->clearVectors();
	d_vectors.clear();
	d_tcu = savedTcu;

	d_capsules.clear();

	if ( !d_tcu
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false) {
		return;
	}

	auto identification = d_experiment->identifier()->identify(d_tcu->TagValue(),
	                                                           d_tcu->Frame().Time());
	if ( !identification ) {
		return;
	}
	// we need AntToOrig
	// we have origToTag and AntToTag
	fmp::Isometry2Dd tagToOrig(d_tcu->TagAngle(),d_tcu->TagPosition());
	auto antToOrig = tagToOrig * identification->AntToTagTransform();
	for ( const auto & [stID,c] : d_experiment->selectedAnt()->capsules() ) {
		qWarning() << "Got " << ToQString(c);
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
		        &AntGeometryWorkspace::onCapsuleUpdated);
	}

	setColorFromType(typeFromComboBox());
}


void AntGeometryWorkspace::setMeasureMode() {
	d_mode = Mode::Measure;
	auto savedTcu = d_tcu;
	d_tcu.reset();
	d_ui->comboBox->setModel(d_experiment->measurements()->typeModel());
	on_comboBox_currentIndexChanged(d_ui->comboBox->currentIndex());
	d_vectorialScene->clearCapsules();
	d_capsules.clear();
	d_tcu = savedTcu;

	d_vectors.clear();

	if ( !d_tcu ) {
		return;
	}


	for ( size_t i = 0; i < d_ui->comboBox->count(); ++i) {
		auto mType = d_ui->comboBox->itemData(i,Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
		if ( !mType ) {
			continue;
		}
		auto m = d_experiment->measurements()->measurementForCloseUp(d_tcu->URI(),
		                                                             mType->MTID());
		if ( !m ) {
			continue;
		}

		fmp::Isometry2Dd tagToOrig(d_tcu->TagAngle(),d_tcu->TagPosition());
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
		        &AntGeometryWorkspace::onVectorUpdated);
	}

	setColorFromType(typeFromComboBox());
}

void AntGeometryWorkspace::onAntSelected(bool antSelected) {
	setTagCloseUp(fmp::TagCloseUp::ConstPtr());
	if ( isEnabled() == false ) {
		return;
	}
	buildCloseUpList();
	updateCloneAction();

	auto ant = d_closeUps->itemFromIndex(d_closeUps->index(0,0));
	if ( ant == nullptr || ant->rowCount() == 0) {
		return;
	}
	int idx = -1;
	for ( int i = 0; i < ant->rowCount(); ++i ) {
		auto item = d_closeUps->itemFromIndex(d_closeUps->index(i,1,ant->index()));
		if ( item == nullptr ) {
			continue;
		}
		if ( item->text().isEmpty() == false ) {
			idx = i;
			break;
		}
	}
	idx = std::max(0,idx);;
	auto index = d_closeUps->index(idx,0,ant->index());
	auto sModel = d_ui->treeView->selectionModel();
	sModel->clear();
	sModel->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows);
	on_treeView_activated(index);
}


void AntGeometryWorkspace::changeEvent(QEvent * event)  {
	QWidget::changeEvent(event);
	if ( event->type() == QEvent::EnabledChange && isEnabled() == true ) {
		buildCloseUpList();
		updateCloneAction();
	}
}

void AntGeometryWorkspace::buildHeaders() {
	auto measurementTypes = d_experiment->measurements()->typeModel();
	QStringList labels;
	labels.reserve(1 + measurementTypes->rowCount());
	labels.push_back(tr("Name"));
	for ( size_t i = 0; i < measurementTypes->rowCount(); ++i ) {
		auto name = measurementTypes->data(measurementTypes->index(i,0),Qt::DisplayRole).toString();
		labels.push_back(tr("%1 count").arg(name));
	}
	d_closeUps->setHorizontalHeaderLabels(labels);
}

void AntGeometryWorkspace::buildCloseUpList() {
	d_closeUps->clear();


	if ( d_experiment->selectedAnt()->isActive() == false ) {
		return;
	}
	auto formatedAntID = fmp::Ant::FormatID(d_experiment->selectedAnt()->selectedID());

	auto measurementTypes = d_experiment->measurements()->typeModel();
	buildHeaders();

	std::map<quint32,int> mTypeIDs;
	for ( size_t i = 0; i < measurementTypes->rowCount(); ++i ) {
		mTypeIDs.insert(std::make_pair(measurementTypes->data(measurementTypes->index(i,1),Qt::DisplayRole).toInt(),0));
	}

	auto ant = new QStandardItem(QString("Ant %1").arg(formatedAntID.c_str()));
	ant->setData(QVariant::fromValue(fmp::TagCloseUp::ConstPtr()));
	ant->setEditable(false);

	auto tcus = d_experiment->tagCloseUps()->closeUpsForAnt(d_experiment->selectedAnt()->selectedID());

	std::sort(tcus.begin(),tcus.end(),
	          [](const fmp::TagCloseUp::ConstPtr & a,
	             const fmp::TagCloseUp::ConstPtr & b) -> bool {
		          return a->Frame().Time().Before(b->Frame().Time());
	          });

	for ( const auto & tcu : tcus ) {
		QList<QStandardItem*> newRow;
		newRow.reserve(1 + measurementTypes->rowCount());
		auto tcuItem = new QStandardItem(ToQString(tcu->URI()));
		auto data = QVariant::fromValue(tcu);
		tcuItem->setData(data);
		tcuItem->setEditable(false);
		newRow.append(tcuItem);

		for ( auto & [mTypeID,count] : mTypeIDs ) {
			auto m = d_experiment->measurements()->measurementForCloseUp(tcu->URI(),
			                                                             mTypeID);


			auto colItem = new QStandardItem(!m ? "" : "1");
			colItem->setData(data);
			colItem->setEditable(false);

			if ( m ) {
				++count;
			}
			newRow.append(colItem);
		}
		ant->appendRow(newRow);
	}

	QList<QStandardItem*> antRow({ant});
	antRow.reserve(1 + measurementTypes->rowCount());
	for ( const auto & [mTypeID,count] : mTypeIDs ) {
		auto countItem = new QStandardItem(QString::number(count));
		countItem->setEditable(false);
		countItem->setData(QVariant::fromValue(fmp::TagCloseUp::ConstPtr()));
		countItem->setData(QVariant::fromValue(mTypeID),Qt::UserRole+2);
		antRow.push_back(countItem);
	}

	d_closeUps->appendRow(antRow);
	d_ui->treeView->expand(d_closeUps->index(0,0));

}

void AntGeometryWorkspace::onMeasurementModification(const QString & tcuURI, quint32 mtID,int direction) {
	auto items = d_closeUps->findItems(tcuURI,Qt::MatchExactly | Qt::MatchRecursive);
	int col = columnForMeasurementType(mtID);
	if ( col < 0 || items.isEmpty() == true ) {
		return;
	}
	auto itemCount = d_closeUps->itemFromIndex(d_closeUps->index(items[0]->row(),col,d_closeUps->index(0,0)));
	auto totalCountItem = d_closeUps->itemFromIndex(d_closeUps->index(0,col));
	if ( direction < 0 ) {
		if (itemCount->text().isEmpty() == true ) {
			return;
		}
		itemCount->setText("");
		totalCountItem->setText(QString::number(totalCountItem->data(Qt::DisplayRole).toInt()-1));
		return;
	}

	if (itemCount->text().isEmpty() == false ) {
		return;
	}
	itemCount->setText("1");
	totalCountItem->setText(QString::number(totalCountItem->data(Qt::DisplayRole).toInt()+1));
}

void AntGeometryWorkspace::onMeasurementModified(const fmp::Measurement::ConstPtr & m) {
	if ( !m ) {
		return;
	}
	auto tcuURI = fs::path(m->URI()).parent_path().parent_path().generic_string();
	onMeasurementModification(ToQString(tcuURI),m->Type(),+1);
}

void AntGeometryWorkspace::onMeasurementDeleted(const fmp::Measurement::ConstPtr & m) {
		if ( !m ) {
		return;
	}
	auto tcuURI = fs::path(m->URI()).parent_path().parent_path().generic_string();
	onMeasurementModification(ToQString(tcuURI),m->Type(),-1);
}



void AntGeometryWorkspace::on_treeView_activated(const QModelIndex & index) {
	auto item = d_closeUps->itemFromIndex(index);
	if ( d_experiment == nullptr || item == nullptr ) {
		return;
	}
	auto tcu = item->data().value<fmp::TagCloseUp::ConstPtr>();
	if ( !tcu ) {
		return;
	}
	setTagCloseUp(tcu);
}


void AntGeometryWorkspace::onIdentificationAntPositionChanged(fmp::Identification::ConstPtr identification) {
	if ( !d_tcu
	     || identification->TagValue() != d_tcu->TagValue()
	     || identification->IsValid(d_tcu->Frame().Time()) == false ) {
		return;
	}
	Eigen::Vector2d position;
	double angle;
	identification->ComputePositionFromTag(position,angle,d_tcu->TagPosition(),d_tcu->TagAngle());
	d_vectorialScene->setPoseIndicator(QPointF(position.x(),
	                                           position.y()),
	                                   angle);
}

void AntGeometryWorkspace::onIdentificationDeleted(fmp::IdentificationConstPtr ident) {
	if ( !d_tcu
	     || d_tcu->TagValue() != ident->TagValue()
	     || ident->IsValid(d_tcu->Frame().Time()) == false ) {
		return;
	}
	setTagCloseUp(fmp::TagCloseUp::ConstPtr());
}

void AntGeometryWorkspace::setTagCloseUp(const fmp::TagCloseUp::ConstPtr & tcu) {
	if ( d_tcu == tcu ) {
		return;
	}
	d_tcu = tcu;
	clearScene();

	if ( d_copyTimeAction != nullptr ) {
		d_copyTimeAction->setEnabled(!d_tcu == false);
	}

	if (!d_tcu) {
		d_vectorialScene->setBackgroundPicture("");
		d_vectorialScene->clearStaticPolygon();
		d_ui->vectorialView->setBannerMessage("",QColor());
		return;
	}

	d_vectorialScene->setBackgroundPicture(ToQString(tcu->AbsoluteFilePath().string()));
	auto & tagPosition = tcu->TagPosition();
	d_ui->vectorialView->centerOn(QPointF(tagPosition.x(),tagPosition.y()));
	d_vectorialScene->setStaticPolygon(tcu->Corners(),QColor(255,0,0));
	auto ident = d_experiment->identifier()->identify(tcu->TagValue(),tcu->Frame().Time());
	if ( !ident ) {
		d_vectorialScene->clearPoseIndicator();
	} else {
		onIdentificationAntPositionChanged(ident);
	}


	on_toolBox_currentChanged(d_ui->toolBox->currentIndex());

}

std::map<uint32_t,QSharedPointer<Vector>>::const_iterator AntGeometryWorkspace::findVector(Vector * vector) const {
	return std::find_if(d_vectors.begin(),
	                    d_vectors.end(),
	                    [vector](const std::pair<uint32_t,QSharedPointer<Vector>> & item ) {
		                    return item.second.data() == vector;
	                    });
}


void AntGeometryWorkspace::onVectorRemoved(QSharedPointer<Vector> vector) {
	if ( !d_closeUp ) {
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


void AntGeometryWorkspace::on_insertButton_clicked() {
	switch(d_mode) {
	case Mode::Shape:
		d_vectorialScene->setMode(VectorialScene::Mode::InsertCapsule);
		break;
	case Mode::Measure:
		break;
	};

}
void AntGeometryWorkspace::on_editButton_clicked() {
}


quint32 AntGeometryWorkspace::typeFromComboBox() const {
	if ( auto shapeType = currentAntShapeType() ) {
		return shapeType->TypeID();
	}
	if ( auto mType = currentMeasurementType() ) {
		return mType->MTID();
	}
	return 0;
}

void AntGeometryWorkspace::on_comboBox_currentIndexChanged(int i) {
	quint32 type = typeFromComboBox();
	if ( type == 0 ) {
		return;
	}

	setColorFromType(type);

	if ( !d_tcu ) {
		return;
	}

	for ( const auto item : d_vectorialScene->selectedItems() ) {
		if ( auto v = dynamic_cast<Vector*>(item) ) {
			changeVectorType(v,type);
			continue;
		}
		if ( auto c = dynamic_cast<Capsule*>(item) ) {
			changeCapsuleType(c,type);
			continue;
		}
	}

}


void AntGeometryWorkspace::onCapsuleUpdated() {
	if ( !d_tcu
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false) {
		return;
	}
	rebuildCapsules();
}

void AntGeometryWorkspace::onCapsuleCreated(QSharedPointer<Capsule> capsule) {
	if ( !d_tcu
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false ) {
		qDebug() << "[AntGeometryWorkspace]: Vector created without tcu or selected ant";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	};

	auto shapeType = currentAntShapeType();
	if ( !shapeType ) {
		qDebug() << "No shape type selected";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	}

	auto c = capsuleFromScene(capsule);
	if ( !c )  {
		qDebug() << "Could not compute capsule, removing it";
		d_vectorialScene->deleteShape(capsule.staticCast<Shape>());
		return;
	}

	connect(capsule.data(),
	        &Shape::updated,
	        this,
	        &AntGeometryWorkspace::onCapsuleUpdated);

	d_capsules.insert(std::make_pair(capsule,shapeType->TypeID()));

	d_experiment->selectedAnt()->addCapsule(shapeType->TypeID(),c);

}

void AntGeometryWorkspace::onCapsuleRemoved(QSharedPointer<Capsule> capsule) {
	if ( !d_tcu
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false ) {
		return;
	}
	auto fi = d_capsules.find(capsule);
	if ( fi == d_capsules.end() ) {
		qDebug() << "[AntGeometryWorkspace]: Could not find back capsule";
		return;
	}

	d_capsules.erase(fi);
	rebuildCapsules();
}

void AntGeometryWorkspace::clearScene() {
	auto savedTcu = d_tcu;
	d_tcu.reset();
	d_vectorialScene->clearVectors();
	d_vectorialScene->clearCapsules();
	d_vectors.clear();
	d_capsules.clear();
	d_tcu = savedTcu;
}



void AntGeometryWorkspace::changeCapsuleType(Capsule * capsule,fmp::AntShapeTypeID stID) {
	auto fi = std::find_if(d_capsules.begin(),
	                       d_capsules.end(),
	                       [capsule](const std::pair<QSharedPointer<Capsule>,uint32_t> & iter) -> bool {
		                       return iter.first.data() == capsule;
	                       });
	if ( !d_tcu
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false
	     || fi == d_capsules.end() ) {
		return;
	}

	fi->second = stID;
	fi->first->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(stID)));
	d_vectorialScene->update();
	rebuildCapsules();
}

fmp::MeasurementTypeConstPtr AntGeometryWorkspace::currentMeasurementType() const {
	return d_ui->comboBox->currentData(Qt::UserRole+1).value<fmp::MeasurementType::Ptr>();
}

fmp::AntShapeTypeConstPtr AntGeometryWorkspace::currentAntShapeType() const {
	return d_ui->comboBox->currentData(Qt::UserRole+1).value<fmp::AntShapeType::Ptr>();
}


int AntGeometryWorkspace::columnForMeasurementType(fmp::MeasurementTypeID mtID) const {
	for ( int i = 1; i < d_closeUps->columnCount(); ++i ) {
		auto item = d_closeUps->itemFromIndex(d_closeUps->index(0,i));
		if ( item != nullptr && item->data(Qt::UserRole+2).toInt() == mtID ) {
			return i;
		}
	}
	return -1;
}

fmp::CapsulePtr AntGeometryWorkspace::capsuleFromScene(const QSharedPointer<Capsule> & capsule) {
	if ( !d_tcu
	     || d_experiment == nullptr) {
		return fmp::CapsulePtr();
	}
	auto ident = d_experiment->identifier()->identify(d_tcu->TagValue(),
	                                                  d_tcu->Frame().Time());
	if ( !ident ) {
		qDebug() << "[AntEditorWdiget]: No Identification for " << ToQString(d_tcu->URI());
		return fmp::CapsulePtr();
	}

	// we need origToAnt
	// we have origToTag and antToTag
	auto origToAnt = ident->AntToTagTransform().inverse() * d_tcu->ImageToTag();
	Eigen::Vector2d c1 = origToAnt * ToEigen(capsule->c1Pos());
	Eigen::Vector2d c2 = origToAnt * ToEigen(capsule->c2Pos());

	return std::make_shared<fmp::Capsule>(c1,c2,capsule->r1(),capsule->r2());
}

void AntGeometryWorkspace::rebuildCapsules() {
	updateCloneAction();

	if ( d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false ){
		return;
	}
	d_experiment->selectedAnt()->clearCapsules();
	for ( const auto & [sceneCapsule,stID] : d_capsules ) {
		auto c = capsuleFromScene(sceneCapsule);
		if ( !c ) {
			continue;
		}
		d_experiment->selectedAnt()->addCapsule(stID,c);
	}
}

void AntGeometryWorkspace::select(int increment) {
	if ( d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false) {
		return;
	}
	auto ant = d_closeUps->itemFromIndex(d_closeUps->index(0,0));
	if ( ant == nullptr ) {
		return;
	}
	int idx = -1;
	if ( d_ui->treeView->selectionModel()->hasSelection() == false ) {
		idx = 0;
		return;
	} else {
		auto rows = d_ui->treeView->selectionModel()->selectedRows();
		if ( rows.isEmpty() == true || rows[0].parent().isValid() == false ) {
			idx = 0;
		} else {
			idx = rows[0].row();
		}
	}
	idx = std::min(std::max(idx+increment,0),ant->rowCount());
	auto sModel = d_ui->treeView->selectionModel();
	sModel->clear();
	auto index = d_closeUps->index(idx,0,ant->index());
	sModel->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows);
	on_treeView_activated(index);

}

void AntGeometryWorkspace::nextCloseUp() { select(+1); }
void AntGeometryWorkspace::previousCloseUp() { select(-1); }

void AntGeometryWorkspace::setUp(const NavigationAction & actions ) {
	connect(actions.NextCloseUp,&QAction::triggered,
	        this,&AntGeometryWorkspace::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        this,&AntGeometryWorkspace::previousCloseUp);


	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&AntGeometryWorkspace::onCopyTime);

	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);

	actions.CopyCurrentTime->setEnabled(!d_tcu == false);
	d_copyTimeAction = actions.CopyCurrentTime;
}

void AntGeometryWorkspace::tearDown(const NavigationAction & actions ) {
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           this,&AntGeometryWorkspace::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           this,&AntGeometryWorkspace::previousCloseUp);
	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&AntGeometryWorkspace::onCopyTime);

	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyTimeAction = nullptr;
}

void AntGeometryWorkspace::onCopyTime() {
	if ( !d_tcu == true ) {
		return;
	}

	QApplication::clipboard()->setText(ToQString(d_tcu->Frame().Time()));
}

void AntGeometryWorkspace::updateCloneAction() {
	if ( this->isEnabled() == false
	     || d_experiment == nullptr
	     || d_experiment->selectedAnt()->isActive() == false
	     || !d_tcu == true
	     || d_mode != Mode::Shape ) {
		d_cloneShapeAction->setEnabled(false);
		return;
	}

	auto selectedAnt = d_experiment->selectedAnt();
	d_cloneShapeAction->setEnabled(selectedAnt->capsules().empty() == false);
}

QAction * AntGeometryWorkspace::cloneAntShapeAction() const {
	return d_cloneShapeAction;
}

void AntGeometryWorkspace::onCloneShapeActionTriggered() {
	if ( d_experiment == nullptr ) {
		return;
	}
	auto opts = CloneShapeDialog::get(d_experiment,
	                                  this);

	if ( !opts == true ) {
		qWarning() << "Not cloning ants";
		return;
	}

	d_experiment->selectedAnt()->cloneShape(opts->ScaleToSize,opts->OverwriteShapes);

}
