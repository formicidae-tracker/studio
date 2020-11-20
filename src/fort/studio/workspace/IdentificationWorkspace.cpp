#include "IdentificationWorkspace.hpp"
#include "ui_IdentificationWorkspace.h"

#include <QKeyEvent>
#include <QClipboard>
#include <QToolBar>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QMainWindow>
#include <QDockWidget>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/GlobalPropertyBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/Utils.hpp>
#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/vectorgraphics/Vector.hpp>
#include <fort/studio/widget/TagCloseUpExplorer.hpp>
#include <fort/studio/widget/IdentificationListWidget.hpp>
#include <fort/studio/widget/TagStatisticsWidget.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>




IdentificationWorkspace::IdentificationWorkspace(QWidget *parent)
	: Workspace(false,parent)
	, d_ui(new Ui::IdentificationWorkspace)
	, d_measurements(nullptr)
	, d_identifier(nullptr)
	, d_statistics(nullptr)
	, d_vectorialScene(new VectorialScene)
	, d_newAntAction(nullptr)
	, d_addIdentificationAction(nullptr)
	, d_deletePoseAction(nullptr)
	, d_copyTimeAction(nullptr)
	, d_actionToolBar(new QToolBar(this))
	, d_navigationToolBar(nullptr) {


#define set_action(res,symbolStr,legendStr,shortCutStr,toolTipStr) do { \
		(res) = d_actionToolBar->addAction(QIcon::fromTheme(symbolStr), \
		                                   tr(legendStr)); \
		(res)->setShortcut(QKeySequence(tr(shortCutStr))); \
		(res)->setToolTip(tr(toolTipStr " (" shortCutStr ")")); \
		(res)->setStatusTip((res)->toolTip()); \
	}while(0);

	set_action(d_newAntAction,
	           "contact-new-symbolic",
	           "New Ant From Close-Up",
	           "Ctrl+A",
	           "Create a new ant from current close-up");
	set_action(d_addIdentificationAction,
	           "address-book-new-symbolic",
	           "Add Identification To...",
	           "Ctrl+I",
	           "Add a new identifcation from current close-up to an existing ant");

	set_action(d_deletePoseAction,
	           "edit-delete-symbolic",
	           "Delete Pose Estimation",
	           "Ctrl+Shift+D",
	           "Deletes current pose estimation");
#undef set_action

	connect(d_newAntAction,&QAction::triggered,
	        this,&IdentificationWorkspace::newAnt);

	connect(d_addIdentificationAction,&QAction::triggered,
	        this,&IdentificationWorkspace::addIdentification);

	connect(d_deletePoseAction,&QAction::triggered,
	        this,&IdentificationWorkspace::deletePose);

    d_ui->setupUi(this);


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



    d_tagExplorer = new QDockWidget(tr("Tag Close-Ups"),this);
    auto tagExplorer = new TagCloseUpExplorer(this);
    d_tagExplorer->setWidget(tagExplorer);
	connect(tagExplorer,
	        &TagCloseUpExplorer::currentCloseUpChanged,
	        this,
	        &IdentificationWorkspace::setTagCloseUp);
	connect(tagExplorer,
	        &TagCloseUpExplorer::currentTagIDChanged,
	        this,
	        &IdentificationWorkspace::onTagIDChanged);


	auto identificationList = new IdentificationListWidget(this);
	d_identificationList = new QDockWidget(tr("Identifications"),this);
	d_identificationList->setWidget(identificationList);
	//TODO connection

	connect(identificationList,
	        &IdentificationListWidget::identificationSelected,
	        tagExplorer,
	        &TagCloseUpExplorer::selectCloseUpForIdentification);


	auto tagStatistics = new TagStatisticsWidget(this);
	d_tagStatistics = new QDockWidget(tr("Tag Statistics"),this);
	d_tagStatistics->setWidget(tagStatistics);


    updateActionStates();
}

IdentificationWorkspace::~IdentificationWorkspace() {
    delete d_ui;
    d_tcu.reset();
}


void IdentificationWorkspace::initialize(QMainWindow * main,ExperimentBridge * experiment) {

	auto globalProperties = experiment->globalProperties();
	auto identifier = experiment->identifier();
	auto measurements = experiment->measurements();



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

	main->addToolBar(d_actionToolBar);
	d_actionToolBar->hide();


	d_statistics = experiment->statistics();

	dynamic_cast<TagCloseUpExplorer*>(d_tagExplorer->widget())->initialize(experiment->tagCloseUps());

	main->addDockWidget(Qt::LeftDockWidgetArea,d_tagExplorer);
	d_tagExplorer->hide();

	dynamic_cast<IdentificationListWidget*>(d_identificationList->widget())->initialize(experiment->identifier());
	main->addDockWidget(Qt::RightDockWidgetArea,d_identificationList);
	d_identificationList->hide();

	main->addDockWidget(Qt::RightDockWidgetArea,d_tagStatistics);
	d_tagStatistics->hide();

}


void IdentificationWorkspace::addIdentification() {
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

	//TODO dialog to ask for an existing ant.

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


void IdentificationWorkspace::setTagCloseUp(const fmp::TagCloseUpConstPtr & tcu) {
	if ( d_tcu == tcu ) {
		return;
	}
	setCloseUpLabels(tcu);
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

void IdentificationWorkspace::setUp(const NavigationAction & actions ) {
	auto tagExplorer = dynamic_cast<TagCloseUpExplorer*>(d_tagExplorer->widget());
	connect(actions.NextTag,&QAction::triggered,
	        tagExplorer,&TagCloseUpExplorer::nextTag);
	connect(actions.PreviousTag,&QAction::triggered,
	        tagExplorer,&TagCloseUpExplorer::previousTag);
	connect(actions.NextCloseUp,&QAction::triggered,
	        tagExplorer,&TagCloseUpExplorer::nextTagCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        tagExplorer,&TagCloseUpExplorer::previousTagCloseUp);

	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&IdentificationWorkspace::onCopyTime);


	actions.NextTag->setEnabled(true);
	actions.PreviousTag->setEnabled(true);
	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);

	actions.CopyCurrentTime->setEnabled(!d_tcu == false);
	d_copyTimeAction = actions.CopyCurrentTime;


	d_actionToolBar->show();
	actions.NavigationToolBar->show();
	d_tagExplorer->show();
	d_identificationList->show();
	d_tagStatistics->show();

}

void IdentificationWorkspace::tearDown(const NavigationAction & actions ) {
	auto tagExplorer = dynamic_cast<TagCloseUpExplorer*>(d_tagExplorer->widget());
	disconnect(actions.NextTag,&QAction::triggered,
	           tagExplorer,&TagCloseUpExplorer::nextTag);
	disconnect(actions.PreviousTag,&QAction::triggered,
	           tagExplorer,&TagCloseUpExplorer::previousTag);
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           tagExplorer,&TagCloseUpExplorer::nextTagCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           tagExplorer,&TagCloseUpExplorer::previousTagCloseUp);

	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&IdentificationWorkspace::onCopyTime);


	actions.NextTag->setEnabled(false);
	actions.PreviousTag->setEnabled(false);
	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyTimeAction = nullptr;

	d_actionToolBar->hide();
	actions.NavigationToolBar->hide();
	d_tagExplorer->hide();
	d_identificationList->hide();
	d_tagStatistics->hide();
}

void IdentificationWorkspace::onCopyTime() {
	if ( !d_tcu ) {
		return;
	}
	QApplication::clipboard()->setText(ToQString(d_tcu->Frame().Time()));
}

void IdentificationWorkspace::onTagIDChanged(int tagID) {
	auto tagStatistics = dynamic_cast<TagStatisticsWidget*>(d_tagStatistics->widget());
	if (tagID == -1
	    || d_statistics->isActive() == false ) {
		tagStatistics->clear();
	} else {
		tagStatistics->display(tagID,d_statistics->statsForTag(tagID),d_statistics->frameCount());
	}
}

void IdentificationWorkspace::setCloseUpLabels(const fmp::TagCloseUp::ConstPtr & closeUp ) {
	QString tagID = tr("N.A.");
	QString time = tagID;
	QString URI = tagID;
	if ( closeUp ) {
		tagID = fm::FormatTagID(closeUp->TagValue()).c_str();
		time = ToQString(closeUp->Frame().Time());
		URI = closeUp->URI().c_str();
	}
	d_ui->closeUpTagLabel->setText(tr("Tag ID: %1").arg(tagID));
	d_ui->closeUpTimeLabel->setText(tr("Time: %1").arg(time));
	d_ui->closeUpURILabel->setText(tr("URI: %1").arg(URI));

}
