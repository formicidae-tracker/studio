#include "VisualizationWidget.hpp"
#include "ui_VisualizationWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include "TrackingVideoPlayer.hpp"


VisualizationWidget::VisualizationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VisualizationWidget)
	, d_videoPlayer(new TrackingVideoPlayer(this)) {
	d_ui->setupUi(this);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::displayVideoFrame,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::display);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::seekReady,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hideLoadingBanner);
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

}
