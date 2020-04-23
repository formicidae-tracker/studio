#include "VisualizationWidget.hpp"
#include "ui_VisualizationWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include "TrackingVideoPlayer.hpp"

#include <QAction>
#include <QShortcut>
#include <QClipboard>

#include <fort/studio/Format.hpp>


VisualizationWidget::VisualizationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VisualizationWidget)
	, d_videoPlayer(new TrackingVideoPlayer(this)) {
	d_ui->setupUi(this);

	auto togglePlayPauseShortcut = new QShortcut(tr("Space"),this);
	auto nextFrameShortcut = new QShortcut(tr("."),this);
	auto prevFrameShortcut = new QShortcut(tr(","),this);

	auto skipForwardSmallShortcut = new QShortcut(tr("L"),this);
	auto skipBackwardSmallShortcut = new QShortcut(tr("J"),this);

	auto skipForwardMediumShortcut = new QShortcut(tr("Shift+L"),this);
	auto skipBackwardMediumShortcut = new QShortcut(tr("Shift+J"),this);

	auto skipForwardLargeShortcut = new QShortcut(tr("Ctrl+Shift+L"),this);
	auto skipBackwardLargeShortcut = new QShortcut(tr("Ctrl+Shift+J"),this);

	static fm::Duration small = 10 * fm::Duration::Second;
	static fm::Duration medium = 1 * fm::Duration::Minute;
	static fm::Duration large = 10 * fm::Duration::Minute;

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::displayVideoFrame,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::display);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::seekReady,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hideLoadingBanner);

	connect(d_ui->trackingVideoWidget,&TrackingVideoWidget::togglePlayPause,
	        d_videoPlayer,&TrackingVideoPlayer::togglePlayPause);

	connect(togglePlayPauseShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::togglePlayPause);

	connect(nextFrameShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::jumpNextFrame);

	connect(prevFrameShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::jumpPrevFrame);

	connect(skipForwardSmallShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(small);
	             });

	connect(skipBackwardSmallShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-small);
	             });

	connect(skipForwardMediumShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(medium);
	             });

	connect(skipBackwardMediumShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-medium);
	             });

	connect(skipForwardLargeShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(large);
	             });

	connect(skipBackwardLargeShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-large);
	             });


}

VisualizationWidget::~VisualizationWidget() {
	delete d_ui;
}

void VisualizationWidget::setup(ExperimentBridge * experiment) {
	auto movieBridge = experiment->movies();

	d_ui->treeView->setModel(movieBridge->movieModel());
	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	connect(movieBridge->movieModel(),
	        &QAbstractItemModel::rowsInserted,
	        d_ui->treeView,
	        &QTreeView::expandAll);
	d_ui->treeView->expandAll();


	connect(d_ui->treeView,
	        &QAbstractItemView::activated,
	        [this,movieBridge] ( const QModelIndex & index ) {
		        const auto & [spaceID,tdd,segment,start]  = movieBridge->tddAndMovieSegment(index);
		        if ( !segment == true || !tdd == true || spaceID == 0) {
			        return;
		        }
		        d_videoPlayer->setMovieSegment(spaceID,tdd,segment,start);
	        });

	d_videoPlayer->setup(experiment->identifiedFrameLoader());
	d_ui->trackingVideoWidget->setup(experiment->identifier());
	d_ui->videoControl->setup(d_videoPlayer,experiment->identifier());

	connect(d_ui->videoControl,
	        &TrackingVideoControl::zoomFocusChanged,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setZoomFocus);

	connect(d_ui->videoControl,
	        &TrackingVideoControl::showID,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setShowID);

	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::showIDChanged,
	        d_ui->videoControl,
	        &TrackingVideoControl::setShowID);

	connect(d_ui->videoControl,
	        &TrackingVideoControl::showInteractions,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setShowInteractions);

	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::showInteractionsChanged,
	        d_ui->videoControl,
	        &TrackingVideoControl::setShowInteractions);

	d_ui->videoControl->setShowID(d_ui->trackingVideoWidget->showID());
}


void VisualizationWidget::onCopyTimeActionTriggered() {
	if ( d_ui->trackingVideoWidget->hasTrackingTime() == false ) {
		return;
	}
	auto time = d_ui->trackingVideoWidget->trackingTime();
	QApplication::clipboard()->setText(ToQString(time));
}

void VisualizationWidget::setUp(const NavigationAction & actions) {
	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hasTrackingTimeChanged,
	        actions.CopyCurrentTime,
	        &QAction::setEnabled);

	connect(actions.CopyCurrentTime,
	        &QAction::triggered,
	        this,
	        &VisualizationWidget::onCopyTimeActionTriggered);

	actions.CopyCurrentTime->setEnabled(d_ui->trackingVideoWidget->hasTrackingTime());
}

void VisualizationWidget::tearDown(const NavigationAction & actions) {
	disconnect(d_ui->trackingVideoWidget,
	           &TrackingVideoWidget::hasTrackingTimeChanged,
	           actions.CopyCurrentTime,
	           &QAction::setEnabled);

	disconnect(actions.CopyCurrentTime,
	           &QAction::triggered,
	           this,
	           &VisualizationWidget::onCopyTimeActionTriggered);
	actions.CopyCurrentTime->setEnabled(false);
}
