#include "VisualizationWidget.hpp"
#include "ui_VisualizationWidget.h"

#include <fort/studio/bridge/MovieBridge.hpp>

#include "TrackingVideoPlayer.hpp"


VisualizationWidget::VisualizationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::VisualizationWidget)
	, d_videoPlayer(new TrackingVideoPlayer(this)) {
	d_ui->setupUi(this);

	d_videoPlayer->setVideoOutput(d_ui->trackingVideoWidget);

}

VisualizationWidget::~VisualizationWidget() {
	delete d_ui;
}

void VisualizationWidget::setup(MovieBridge * bridge) {
	d_ui->treeView->setModel(bridge->movieModel());
	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	connect(bridge->movieModel(),
	        &QAbstractItemModel::rowsInserted,
	        d_ui->treeView,
	        &QTreeView::expandAll);
	d_ui->treeView->expandAll();


	connect(d_ui->treeView,
	        &QAbstractItemView::activated,
	        [this,bridge] ( const QModelIndex & index ) {
		        auto segmentAndTime = bridge->movieSegment(index);
		        auto segment = std::get<0>(segmentAndTime);
		        if ( !segment ) {
			        return;
		        }
		        d_videoPlayer->setMovieSegment(segment,std::get<1>(segmentAndTime),std::get<2>(segmentAndTime));
	        });
}
