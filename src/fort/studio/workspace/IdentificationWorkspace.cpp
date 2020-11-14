#include "IdentificationWorkspace.hpp"
#include "ui_IdentificationWorkspace.h"

#include <QKeyEvent>
#include <QClipboard>

#include <QSortFilterProxyModel>
#include <QAction>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/GlobalPropertyBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/SelectedAntBridge.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/Utils.hpp>
#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/vectorgraphics/Vector.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>



IdentificationWorkspace::IdentificationWorkspace(QWidget *parent)
	: Workspace(false,parent)
	, d_ui(new Ui::IdentificationWorkspace)
	, d_sortedModel ( new QSortFilterProxyModel(this) )
	, d_measurements(nullptr)
	, d_identifier(nullptr)
	, d_vectorialScene(new VectorialScene)
	, d_newAntAction(new QAction(tr("New Ant from Tag"),this))
	, d_addIdentificationAction(new QAction(tr("Add Identification to Ant"),this))
	, d_deletePoseAction(new QAction(tr("Delete Pose Estimation"),this))
	, d_copyTimeAction(nullptr) {

	d_newAntAction->setShortcut(QKeySequence(tr("Ctrl+A")));
	d_addIdentificationAction->setShortcut(QKeySequence(tr("Ctrl+I")));
	d_deletePoseAction->setShortcut(QKeySequence(tr("Ctrl+Shift+D")));

	connect(d_newAntAction,&QAction::triggered,
	        this,&IdentificationWorkspace::newAnt);

	connect(d_addIdentificationAction,&QAction::triggered,
	        this,&IdentificationWorkspace::addIdentification);

	connect(d_deletePoseAction,&QAction::triggered,
	        this,&IdentificationWorkspace::deletePose);

	d_sortedModel->setSortRole(Qt::UserRole+2);
    d_ui->setupUi(this);

	d_ui->newAntButton->setDefaultAction(d_newAntAction);
	d_ui->addIdentButton->setDefaultAction(d_addIdentificationAction);
	d_ui->deletePoseButton->setDefaultAction(d_deletePoseAction);


    d_ui->treeView->setModel(d_sortedModel);
    d_ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    d_ui->vectorialView->setScene(d_vectorialScene);
    d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
    connect(d_ui->vectorialView,
            &VectorialView::zoomed,
            d_vectorialScene,
            &VectorialScene::onZoomed);
    d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(fmp::Measurement::HEAD_TAIL_TYPE)));
    connect(d_vectorialScene,
            &VectorialScene::vectorCreated,
            this,
            &IdentificationWorkspace::onVectorCreated);
    connect(d_vectorialScene,
            &VectorialScene::vectorRemoved,
            this,
            &IdentificationWorkspace::onVectorRemoved);

    updateActionStates();
}

IdentificationWorkspace::~IdentificationWorkspace() {
    delete d_ui;
    d_tcu.reset();
}


void IdentificationWorkspace::initialize(ExperimentBridge * experiment) {

	auto globalProperties = experiment->globalProperties();
	auto identifier = experiment->identifier();
	auto measurements = experiment->measurements();
	auto selectedAnt = experiment->selectedAnt();


	connect(d_sortedModel,
	        &QAbstractItemModel::rowsInserted,
	        [this](const QModelIndex & parent, int first, int last) {
		        if ( parent.isValid() == true ) {
			        return;
		        }
		        for ( int i = first; i <= last; ++i) {
			        d_ui->treeView->expand(d_sortedModel->index(i,0,parent));
		        }
	        });


	d_sortedModel->setSourceModel(measurements->tagCloseUpModel());
	d_ui->treeView->sortByColumn(0,Qt::AscendingOrder);
	d_measurements = measurements;

	connect(identifier,
	        &IdentifierBridge::identificationAntPositionModified,
	        this,
	        &IdentificationWorkspace::onIdentificationAntPositionChanged);

	connect(identifier,
	        &IdentifierBridge::identificationCreated,
	        this,
	        &IdentificationWorkspace::onIdentificationAntPositionChanged);
	connect(identifier,
	        &IdentifierBridge::identificationDeleted,
	        this,
	        &IdentificationWorkspace::onIdentificationDeleted);



	d_identifier = identifier;
	setTagCloseUp(fmp::TagCloseUp::Ptr());

	connect(selectedAnt,
	        &Bridge::activated,
	        this,
	        &IdentificationWorkspace::updateActionStates);
	d_selectedAnt = selectedAnt;

	d_ui->selectedAntIdentification->setup(experiment);

}


void IdentificationWorkspace::addIdentification() {
	if ( !d_tcu ) {
		return;
	}
	auto m = d_measurements->measurement(d_tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		return;
	}

	if ( d_selectedAnt->isActive() == false ) {
		return;
	}

	fm::Time::ConstPtr start,end;
	if ( d_identifier->freeRangeContaining(start,end,d_tcu->TagValue(),d_tcu->Frame().Time()) == false ) {
		qCritical() << "TagID:" << fmp::FormatTagID(d_tcu->TagValue()).c_str()
		            << " already identifies an Ant at Time "
		            << ToQString(d_tcu->Frame().Time());
		return;
	}


	d_identifier->addIdentification(d_selectedAnt->selectedID(),
	                                d_tcu->TagValue(),
	                                start,end);

	updateActionStates();
}

void IdentificationWorkspace::newAnt() {
	if ( !d_tcu ) {
		return;
	}
	auto m = d_measurements->measurement(d_tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		return;
	}
	fm::Time::ConstPtr start,end;
	if ( d_identifier->freeRangeContaining(start,end,d_tcu->TagValue(),d_tcu->Frame().Time()) == false ) {
		qCritical() << "TagID:" << fmp::FormatTagID(d_tcu->TagValue()).c_str()
		            << " already identifies an Ant at Time "
		            << ToQString(d_tcu->Frame().Time());
		return;
	}


	auto a = d_identifier->createAnt();
	d_identifier->addIdentification(a->AntID(),
	                                d_tcu->TagValue(),
	                                start,end);

	updateActionStates();
}

void IdentificationWorkspace::deletePose() {
	if ( !d_tcu || d_vectorialScene->vectors().isEmpty() == true ) {
		return;
	}
	for ( const auto & v : d_vectorialScene->vectors() ) {
		d_vectorialScene->deleteShape(v.staticCast<Shape>());
	}
	onVectorRemoved();
}

void IdentificationWorkspace::onIdentificationAntPositionChanged(fmp::Identification::ConstPtr identification) {
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
	updateActionStates();
}


void IdentificationWorkspace::on_treeView_activated(const QModelIndex & index) {
	if ( index.parent().isValid() == false || d_measurements == nullptr ) {
		return;
	}
	auto tcu = d_measurements->fromTagCloseUpModelIndex(d_sortedModel->mapToSource(index));
	if ( !tcu ) {
		return;
	}
	setTagCloseUp(tcu);
}


void IdentificationWorkspace::nextTag() {
	if ( d_ui->treeView->selectionModel()->hasSelection() == false ) {
		selectRow(0,0);
		return;
	}
	qWarning() << d_ui->treeView->selectionModel()->selectedRows();
	auto firstRow = d_ui->treeView->selectionModel()->selectedRows()[0];
	qWarning() << firstRow.parent();
	if ( firstRow.parent().isValid() == false ) {
		selectRow(firstRow.row()+1,0);
		return;
	}
	selectRow(firstRow.parent().row()+1,0);
}

void IdentificationWorkspace::nextTagCloseUp() {
	if ( d_ui->treeView->selectionModel()->hasSelection() == false ) {
		selectRow(0,0);
		return;
	}
	auto firstRow = d_ui->treeView->selectionModel()->selectedRows()[0];
	if ( firstRow.parent().isValid() == false ) {
		selectRow(firstRow.row(),0);
	}
	selectRow(firstRow.parent().row(),firstRow.row()+1);
}

void IdentificationWorkspace::previousTag() {
	if ( d_ui->treeView->selectionModel()->hasSelection() == false ) {
		selectRow(0,0);
		return;
	}
	auto firstRow = d_ui->treeView->selectionModel()->selectedRows()[0];
	if ( firstRow.parent().isValid() == false ) {
		selectRow(firstRow.row()-1,0);
		return;
	}
	selectRow(firstRow.parent().row()-1,0);
}

void IdentificationWorkspace::previousTagCloseUp() {
	if ( d_ui->treeView->selectionModel()->hasSelection() == false ) {
		selectRow(0,0);
		return;
	}
	auto firstRow = d_ui->treeView->selectionModel()->selectedRows()[0];
	if ( firstRow.parent().isValid() == false ) {
		selectRow(firstRow.row(),0);
	}
	selectRow(firstRow.parent().row(),firstRow.row()-1);
}

void IdentificationWorkspace::selectRow(int tagRow, int tcuRow) {
	if (tagRow < 0 || tagRow >= d_sortedModel->rowCount() || tcuRow < 0) {
		return;
	}
	auto tagIndex = d_sortedModel->index(tagRow,0);
	auto tcuCount = d_sortedModel->rowCount(tagIndex);
	auto columnCount = d_sortedModel->columnCount(tagIndex);
	if ( tcuRow >= tcuCount ) {
		return;
	}
	d_ui->treeView->selectionModel()->clearSelection();
	for ( size_t i = 0; i < columnCount; ++i) {
		d_ui->treeView->selectionModel()->select(d_sortedModel->index(tcuRow,i,tagIndex),
		                                         QItemSelectionModel::Select);
	}
	auto index = d_sortedModel->index(tcuRow,0,tagIndex);
	on_treeView_activated(index);
	d_ui->treeView->scrollTo(index);
}


void IdentificationWorkspace::setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu) {
	if ( d_tcu == tcu ) {
		return;
	}

	d_tcu.reset();
	for ( const auto & v : d_vectorialScene->vectors() ) {
		d_vectorialScene->deleteShape(v.staticCast<Shape>());
	}
	d_tcu = tcu;
	if ( d_copyTimeAction != nullptr ) {
		d_copyTimeAction->setEnabled( !d_tcu == false );
	}

	if ( !tcu ) {
		d_vectorialScene->setBackgroundPicture("");
		d_vectorialScene->clearStaticPolygon();
		d_ui->vectorialView->setBannerMessage("",QColor());
		updateActionStates();
		return;
	}

	qInfo() << "Loading " << ToQString(tcu->URI()) << " image " << ToQString(tcu->AbsoluteFilePath());

	double squareness = d_tcu->Squareness();
	const static double threshold = 0.95;
	if ( squareness < threshold ) {
		auto color = Conversion::colorFromFM(fmp::Palette::Default().At(5));
		d_ui->vectorialView->setBannerMessage(tr("WARNING: Tag Squareness is Low (%1 < %2)").arg(squareness).arg(threshold),color);
	} else {
		d_ui->vectorialView->setBannerMessage("",QColor());
	}

	d_vectorialScene->setBackgroundPicture(ToQString(tcu->AbsoluteFilePath().string()));
	auto & tagPosition = tcu->TagPosition();
	d_ui->vectorialView->centerOn(QPointF(tagPosition.x(),tagPosition.y()));
	d_vectorialScene->setStaticPolygon(tcu->Corners(),QColor(255,0,0));
	auto ident = d_identifier->identify(tcu->TagValue(),tcu->Frame().Time());
	if ( !ident ) {
		d_vectorialScene->clearPoseIndicator();
	} else {
		onIdentificationAntPositionChanged(ident);
	}

	setGraphicsFromMeasurement(tcu);
	d_tcu = tcu;
	updateActionStates();
}


void IdentificationWorkspace::setGraphicsFromMeasurement(const fmp::TagCloseUpConstPtr & tcu) {
	auto m = d_measurements->measurement(tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		d_vectorialScene->setOnce(true);
		d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
		d_vectorialScene->clearVectors();
	} else {
		fmp::Isometry2Dd tagToOrig(tcu->TagAngle(),tcu->TagPosition());
		Eigen::Vector2d start = tagToOrig * m->StartFromTag();
		Eigen::Vector2d end = tagToOrig * m->EndFromTag();

		QSharedPointer<Vector> vector;
		if ( d_vectorialScene->vectors().isEmpty() ) {
			vector = d_vectorialScene->appendVector(QPointF(start.x(),
			                                                start.y()),
			                                        QPointF(end.x(),
			                                                end.y()));
			connect(vector.data(),
			        &Shape::updated,
			        this,
			        &IdentificationWorkspace::onVectorUpdated);

		} else {
			vector = d_vectorialScene->vectors()[0];
		}


		d_vectorialScene->setMode(VectorialScene::Mode::Edit);
	}
}


void IdentificationWorkspace::onVectorUpdated() {
	if ( !d_tcu ) {
		qDebug() << "[IdentificationWorkspace]: Vector updated without TCU";
		return;
	}
	if ( d_vectorialScene->vectors().isEmpty() == true ) {
		qDebug() << "[IdentificationWorkspace]: Vector updated without vector";
		return;
	}
	auto vector = d_vectorialScene->vectors()[0];

	d_measurements->setMeasurement(d_tcu,
	                               fmp::Measurement::HEAD_TAIL_TYPE,
	                               vector->startPos(),
	                               vector->endPos());

	setGraphicsFromMeasurement(d_tcu);

	updateActionStates();
}

void IdentificationWorkspace::onVectorCreated(QSharedPointer<Vector> vector) {
	if ( !d_tcu ) {
		qDebug() << "[IdentificationWorkspace]: Vector created without TCU";
		return;
	}

	if ( d_measurements->setMeasurement(d_tcu,
	                                    fmp::Measurement::HEAD_TAIL_TYPE,
	                                    vector->startPos(),
	                                    vector->endPos()) == false ) {

		d_vectorialScene->deleteShape(vector.staticCast<Shape>());
		d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
		updateActionStates();
		return;
	};

	connect(vector.data(),
	        &Shape::updated,
	        this,
	        &IdentificationWorkspace::onVectorUpdated);

	updateActionStates();

}


void IdentificationWorkspace::onVectorRemoved() {
	if ( !d_tcu ) {
		return;
	}
	auto m = d_measurements->measurement(d_tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		qDebug() << "No measurement 'head-tail' for " << ToQString(d_tcu->URI());
		return;
	}
	d_measurements->deleteMeasurement(m->URI());
	d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
	updateActionStates();
}


void IdentificationWorkspace::updateActionStates() {
	if ( !d_tcu || d_vectorialScene->vectors().isEmpty() == true ) {
		d_newAntAction->setEnabled(false);
		d_addIdentificationAction->setEnabled(false);
		d_deletePoseAction->setEnabled(false);
		return;
	}
	d_deletePoseAction->setEnabled(true);

	auto ident = d_identifier->identify(d_tcu->TagValue(),d_tcu->Frame().Time());
	if ( ident ) {
		d_newAntAction->setEnabled(false);
		d_addIdentificationAction->setEnabled(false);
		return;
	}
	d_newAntAction->setEnabled(true);

	if ( d_selectedAnt->isActive() == false ) {
		d_addIdentificationAction->setEnabled(false);
		return;
	}
	d_addIdentificationAction->setEnabled(true);
}

void IdentificationWorkspace::onIdentificationDeleted(fmp::IdentificationConstPtr ident) {
	if ( !d_tcu
	     || d_tcu->TagValue() != ident->TagValue()
	     || ident->IsValid(d_tcu->Frame().Time()) == false ) {
		return;
	}
	d_vectorialScene->clearPoseIndicator();
	updateActionStates();
}


QAction * IdentificationWorkspace::newAntFromTagAction() const {
	return d_newAntAction;
}

QAction * IdentificationWorkspace::addIdentificationToAntAction() const {
	return d_addIdentificationAction;
}

QAction * IdentificationWorkspace::deletePoseEstimationAction() const {
	return d_deletePoseAction;
}

void IdentificationWorkspace::setUp(QMainWindow * main,const NavigationAction & actions ) {
	connect(actions.NextTag,&QAction::triggered,
	        this,&IdentificationWorkspace::nextTag);
	connect(actions.PreviousTag,&QAction::triggered,
	        this,&IdentificationWorkspace::previousTag);
	connect(actions.NextCloseUp,&QAction::triggered,
	        this,&IdentificationWorkspace::nextTagCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        this,&IdentificationWorkspace::previousTagCloseUp);

	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&IdentificationWorkspace::onCopyTime);


	actions.NextTag->setEnabled(true);
	actions.PreviousTag->setEnabled(true);
	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);

	actions.CopyCurrentTime->setEnabled(!d_tcu == false);
	d_copyTimeAction = actions.CopyCurrentTime;
}

void IdentificationWorkspace::tearDown(QMainWindow * main, const NavigationAction & actions ) {
	disconnect(actions.NextTag,&QAction::triggered,
	           this,&IdentificationWorkspace::nextTag);
	disconnect(actions.PreviousTag,&QAction::triggered,
	           this,&IdentificationWorkspace::previousTag);
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           this,&IdentificationWorkspace::nextTagCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           this,&IdentificationWorkspace::previousTagCloseUp);

	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&IdentificationWorkspace::onCopyTime);


	actions.NextTag->setEnabled(false);
	actions.PreviousTag->setEnabled(false);
	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyTimeAction = nullptr;
}

void IdentificationWorkspace::onCopyTime() {
	if ( !d_tcu ) {
		return;
	}
	QApplication::clipboard()->setText(ToQString(d_tcu->Frame().Time()));
}
