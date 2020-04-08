#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"

#include <QKeyEvent>

#include <QSortFilterProxyModel>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/GlobalPropertyBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/SelectedAntBridge.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/Utils.hpp>
#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/vectorgraphics/Vector.hpp>



TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget)
	, d_sortedModel ( new QSortFilterProxyModel(this) )
	, d_measurements(nullptr)
	, d_identifier(nullptr)
	, d_vectorialScene(new VectorialScene) {
	installEventFilter(this);

	d_sortedModel->setSortRole(Qt::UserRole+2);
    d_ui->setupUi(this);


    d_ui->treeView->installEventFilter(this);
    d_ui->treeView->setModel(d_sortedModel);
    d_ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    d_ui->vectorialView->installEventFilter(this);
    d_ui->vectorialView->setScene(d_vectorialScene);
    d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
    d_vectorialScene->installEventFilter(d_ui->vectorialView);
    connect(d_ui->vectorialView,
            &VectorialView::zoomed,
            d_vectorialScene,
            &VectorialScene::onZoomed);
    d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(fmp::Measurement::HEAD_TAIL_TYPE)));
    connect(d_vectorialScene,
            &VectorialScene::vectorCreated,
            this,
            &TaggingWidget::onVectorCreated);
    connect(d_vectorialScene,
            &VectorialScene::vectorRemoved,
            this,
            &TaggingWidget::onVectorRemoved);

    updateButtonStates();
}

TaggingWidget::~TaggingWidget() {
    delete d_ui;
    d_tcu.reset();
}


void TaggingWidget::setup(ExperimentBridge * experiment) {

	auto globalProperties = experiment->globalProperties();
	auto identifier = experiment->identifier();
	auto measurements = experiment->measurements();
	auto selectedAnt = experiment->selectedAnt();

	connect(globalProperties,
	        &GlobalPropertyBridge::activated,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setEnabled);

	connect(globalProperties,
	        &GlobalPropertyBridge::tagFamilyChanged,
	        d_ui->familySelector,
	        &TagFamilyComboBox::setFamily);

	connect(d_ui->familySelector,
	        &TagFamilyComboBox::familyModified,
	        globalProperties,
	        &GlobalPropertyBridge::setTagFamily);


	connect(globalProperties,
	        &GlobalPropertyBridge::activated,
	        d_ui->thresholdBox,
	        &QSpinBox::setEnabled);

	connect(globalProperties,
	        &GlobalPropertyBridge::thresholdChanged,
	        d_ui->thresholdBox,
	        &QSpinBox::setValue);

	connect(d_ui->thresholdBox,
	        &QSpinBox::editingFinished,
	        [globalProperties,this] () {
		        globalProperties->setThreshold(d_ui->thresholdBox->value());
	        });

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
	connect(measurements,
	        &MeasurementBridge::progressChanged,
	        [this](size_t done, size_t toDo) {
		        d_ui->tagCloseUpLoadingProgress->setMaximum(toDo);
		        d_ui->tagCloseUpLoadingProgress->setValue(done);
	        });
	d_measurements = measurements;

	connect(identifier,
	        &IdentifierBridge::identificationAntPositionModified,
	        this,
	        &TaggingWidget::onIdentificationAntPositionChanged);

	connect(identifier,
	        &IdentifierBridge::identificationCreated,
	        this,
	        &TaggingWidget::onIdentificationAntPositionChanged);
	connect(identifier,
	        &IdentifierBridge::identificationDeleted,
	        this,
	        &TaggingWidget::onIdentificationDeleted);



	d_identifier = identifier;
	setTagCloseUp(fmp::TagCloseUp::Ptr());

	connect(selectedAnt,
	        &Bridge::activated,
	        this,
	        &TaggingWidget::updateButtonStates);
	d_selectedAnt = selectedAnt;

	d_ui->selectedAntIdentification->setup(experiment);

}


void TaggingWidget::on_addIdentButton_clicked() {
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
		qCritical() << "TagID:" << d_tcu->TagValue()
		            << " already identifies an Ant at Time "
		            << ToQString(d_tcu->Frame().Time());
		return;
	}


	d_identifier->addIdentification(d_selectedAnt->selectedID(),
	                                d_tcu->TagValue(),
	                                start,end);

	updateButtonStates();
}

void TaggingWidget::on_newAntButton_clicked() {
	if ( !d_tcu ) {
		return;
	}
	auto m = d_measurements->measurement(d_tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		return;
	}
	fm::Time::ConstPtr start,end;
	if ( d_identifier->freeRangeContaining(start,end,d_tcu->TagValue(),d_tcu->Frame().Time()) == false ) {
		qCritical() << "TagID:" << d_tcu->TagValue()
		            << " already identifies an Ant at Time "
		            << ToQString(d_tcu->Frame().Time());
		return;
	}


	auto a = d_identifier->createAnt();
	d_identifier->addIdentification(a->ID(),
	                                d_tcu->TagValue(),
	                                start,end);

	updateButtonStates();
}

void TaggingWidget::on_deletePoseButton_clicked() {
	if ( !d_tcu || d_vectorialScene->vectors().isEmpty() == true ) {
		return;
	}
	for ( const auto & v : d_vectorialScene->vectors() ) {
		d_vectorialScene->deleteShape(v.staticCast<Shape>());
	}
	onVectorRemoved();
}

void TaggingWidget::onIdentificationAntPositionChanged(fmp::Identification::ConstPtr identification) {
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
	updateButtonStates();
}


void TaggingWidget::on_treeView_activated(const QModelIndex & index) {
	if ( index.parent().isValid() == false || d_measurements == nullptr ) {
		return;
	}
	auto tcu = d_measurements->fromTagCloseUpModelIndex(d_sortedModel->mapToSource(index));
	if ( !tcu ) {
		return;
	}
	setTagCloseUp(tcu);
}



bool TaggingWidget::eventFilter(QObject * obj, QEvent * event) {
	if ( event->type() != QEvent::KeyPress ) {
		return false;
	}
	auto keyEvent = static_cast<QKeyEvent*>(event);
	if ( keyEvent->modifiers() == Qt::ControlModifier ) {
		if ( keyEvent->key() == Qt::Key_Down ) {
			nextTag();
			return true;
		}
		if ( keyEvent->key() == Qt::Key_Up ) {
			previousTag();
			return true;
		}
	}

	if ( keyEvent->modifiers() == Qt::ShiftModifier ) {
		if ( keyEvent->key() == Qt::Key_Down ) {
			nextTagCloseUp();
			return true;
		}
		if ( keyEvent->key() == Qt::Key_Up ) {
			previousTagCloseUp();
			return true;
		}
	}

	return false;
}

void TaggingWidget::nextTag() {
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

void TaggingWidget::nextTagCloseUp() {
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

void TaggingWidget::previousTag() {
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

void TaggingWidget::previousTagCloseUp() {
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

void TaggingWidget::selectRow(int tagRow, int tcuRow) {
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


void TaggingWidget::setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu) {
	if ( d_tcu == tcu ) {
		return;
	}

	d_tcu.reset();
	for ( const auto & v : d_vectorialScene->vectors() ) {
		d_vectorialScene->deleteShape(v.staticCast<Shape>());
	}
	d_tcu = tcu;
	if ( !tcu ) {
		d_vectorialScene->setBackgroundPicture("");
		d_vectorialScene->clearStaticPolygon();
		d_ui->vectorialView->setBannerMessage("",QColor());
		updateButtonStates();
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


	auto m = d_measurements->measurement(tcu->URI(),fmp::Measurement::HEAD_TAIL_TYPE);
	if ( !m ) {
		d_vectorialScene->setOnce(true);
		d_vectorialScene->setMode(VectorialScene::Mode::InsertVector);
	} else {
		fmp::Isometry2Dd tagToOrig(tcu->TagAngle(),tcu->TagPosition());
		Eigen::Vector2d start = tagToOrig * m->StartFromTag();
		Eigen::Vector2d end = tagToOrig * m->EndFromTag();

		auto vector = d_vectorialScene->appendVector(QPointF(start.x(),
		                                                     start.y()),
		                                             QPointF(end.x(),
		                                                     end.y()));
		connect(vector.data(),
		        &Shape::updated,
		        this,
		        &TaggingWidget::onVectorUpdated);

		d_vectorialScene->setMode(VectorialScene::Mode::Edit);
	}

	d_tcu = tcu;
	updateButtonStates();
}


void TaggingWidget::onVectorUpdated() {
	if ( !d_tcu ) {
		qDebug() << "[TaggingWidget]: Vector updated without TCU";
		return;
	}
	if ( d_vectorialScene->vectors().isEmpty() == true ) {
		qDebug() << "[TaggingWidget]: Vector updated without vector";
		return;
	}
	auto vector = d_vectorialScene->vectors()[0];
	auto imageToTag = d_tcu->ImageToTag();

	d_measurements->setMeasurement(d_tcu,
	                               fmp::Measurement::HEAD_TAIL_TYPE,
	                               vector->startPos(),
	                               vector->endPos());
	updateButtonStates();
}

void TaggingWidget::onVectorCreated(QSharedPointer<Vector> vector) {
	if ( !d_tcu ) {
		qDebug() << "[TaggingWidget]: Vector created without TCU";
		return;
	}
	d_measurements->setMeasurement(d_tcu,
	                               fmp::Measurement::HEAD_TAIL_TYPE,
	                               vector->startPos(),
	                               vector->endPos());

	connect(vector.data(),
	        &Shape::updated,
	        this,
	        &TaggingWidget::onVectorUpdated);

	updateButtonStates();

}


void TaggingWidget::onVectorRemoved() {
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
	updateButtonStates();
}


void TaggingWidget::updateButtonStates() {
	if ( !d_tcu || d_vectorialScene->vectors().isEmpty() == true ) {
		d_ui->newAntButton->setEnabled(false);
		d_ui->addIdentButton->setEnabled(false);
		d_ui->deletePoseButton->setEnabled(false);
		return;
	}
	d_ui->deletePoseButton->setEnabled(true);

	auto ident = d_identifier->identify(d_tcu->TagValue(),d_tcu->Frame().Time());
	if ( ident ) {
		d_ui->newAntButton->setEnabled(false);
		d_ui->addIdentButton->setEnabled(false);
		return;
	}
	d_ui->newAntButton->setEnabled(true);

	if ( d_selectedAnt->isActive() == false ) {
		d_ui->addIdentButton->setEnabled(false);
		return;
	}
	d_ui->addIdentButton->setEnabled(true);
}

void TaggingWidget::onIdentificationDeleted(fmp::IdentificationConstPtr ident) {
	if ( !d_tcu
	     || d_tcu->TagValue() != ident->TagValue()
	     || ident->IsValid(d_tcu->Frame().Time()) == false ) {
		return;
	}
	d_vectorialScene->clearPoseIndicator();
	updateButtonStates();
}
