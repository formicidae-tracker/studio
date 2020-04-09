#include "VisualizationWidget.hpp"
#include "ui_VisualizationWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include "TrackingVideoPlayer.hpp"

#include <QAction>
#include <QClipboard>

#include <fort/studio/Format.hpp>


VisualizationWidget::VisualizationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VisualizationWidget)
	, d_videoPlayer(new TrackingVideoPlayer(this))
	, d_copyTimeAction(new QAction(tr("Copy current Timestamp"),this)) {
	d_ui->setupUi(this);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::displayVideoFrame,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::display);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::seekReady,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hideLoadingBanner);

	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hasTrackingTimeChanged,
	        d_copyTimeAction,
	        &QAction::setEnabled);

	connect(d_copyTimeAction,
	        &QAction::triggered,
	        this,
	        &VisualizationWidget::onCopyTimeActionTriggered);

	d_copyTimeAction->setShortcut(QKeySequence(tr("Ctrl+Shift+C")));
    d_copyTimeAction->setStatusTip(tr("Copy current Frame timestamp to clipboard"));
    d_copyTimeAction->setEnabled(d_ui->trackingVideoWidget->hasTrackingTime());
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
		        const auto & [tdd,segment,start]  = movieBridge->tddAndMovieSegment(index);
		        if ( !segment == true || !tdd == true) {
			        return;
		        }
		        d_videoPlayer->setMovieSegment(tdd,segment,start);
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
	d_ui->videoControl->setShowID(d_ui->trackingVideoWidget->showID());
}


void VisualizationWidget::onCopyTimeActionTriggered() {
	if ( d_ui->trackingVideoWidget->hasTrackingTime() == false ) {
		return;
	}
	auto time = d_ui->trackingVideoWidget->trackingTime();
	QApplication::clipboard()->setText(ToQString(time));
}

QAction * VisualizationWidget::copyCurrentTimeAction() const {
	return d_copyTimeAction;
}
