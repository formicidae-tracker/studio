#include "SnapshotViewer.hpp"

#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>

SnapshotViewer::SnapshotViewer(QWidget *parent)
	: QGraphicsView(parent)
	, d_roiSize(500)
	, d_background(NULL) {
	d_tagCornerBrush = QBrush(QColor(50,50,50));
	d_tagCornerPen = QPen(QColor(255,255,255));
	d_tagCornerPen.setWidth(1);
	d_tagLinePen = QPen(QColor(255,20,20,120));
	d_tagLinePen.setWidth(TAG_LINE_SIZE);

	for(size_t i = 0;i < 4; ++i ) {
		d_tagCorners[i] = d_scene.addRect(QRect(0,0,TAG_CORNER_POINT_SIZE+2,TAG_CORNER_POINT_SIZE+2),d_tagCornerPen,d_tagCornerBrush);
		d_tagCorners[i]->setVisible(false);
		d_tagCorners[i]->setEnabled(false);

		d_tagCorners[i]->setZValue(2);
		d_tagLines[i] = d_scene.addLine(0,0,0,0,d_tagLinePen);
		d_tagLines[i]->setZValue(1);
		d_tagLines[i]->setVisible(false);
		d_tagLines[i]->setEnabled(false);

	}

	setScene(&d_scene);
	setRoiSize(d_roiSize);

	d_head = new PositionMarker(10.1,17.3,*this);
	d_tail = new PositionMarker(42.0,38.1,*this);
	d_head->setZValue(4);
	d_tail->setZValue(5);
	d_scene.addItem(d_head);
	d_scene.addItem(d_tail);
	d_head->setVisible(false);
	d_head->setEnabled(false);
	d_tail->setVisible(false);
	d_tail->setEnabled(false);


	displaySnapshot(Snapshot::ConstPtr());

	d_estimateLine = new QGraphicsLineItem(0,0,0,0);
	d_estimateLine->setVisible(false);
	QPen linePen(PositionMarker::COLOR);
	linePen.setWidth(3);
	d_estimateLine->setPen(linePen);
	d_estimateLine->setZValue(3);
	d_scene.addItem(d_estimateLine);

}

SnapshotViewer::~SnapshotViewer() {
}


void SnapshotViewer::displaySnapshot(const Snapshot::ConstPtr & s) {
	d_snapshot = s;

	if (!d_snapshot) {
		for(size_t i = 0; i < 4; ++i) {
			d_tagCorners[i]->setVisible(false);
			d_tagLines[i]->setVisible(false);
		}
		return;
	}

	auto imagepath = d_basedir / s->ImagePath();
	d_image = QImage(imagepath.c_str());

	if ( d_image.format() != QImage::Format_RGB888 ) {
		d_image = d_image.convertToFormat(QImage::Format_RGB888);
	}

	setImageBackground();
	setAntPoseEstimate(AntPoseEstimate::Ptr());
}


size_t SnapshotViewer::roiSize() const {
	return d_roiSize;
}

void SnapshotViewer::setRoiSize(size_t roiSize) {
	bool emitSignal = false;
	if ( roiSize != d_roiSize ) {
		emitSignal = true;
	}
	d_roiSize = roiSize;
	setImageBackground();
	if ( emitSignal == true ) {
		emit roiSizeChanged(roiSize);
	}
}

void SnapshotViewer::setBasedir(const std::filesystem::path & path) {
	d_basedir = path;
}


void SnapshotViewer::setImageBackground() {
	if (d_image.width() <= d_roiSize || d_image.height() <= d_roiSize ) {
		d_roi = QRect(0,0,d_image.width(),d_image.height());
		setSceneRect(QRect(0,0,d_image.width(),d_image.height()));
	} else {
		auto tagPosition = d_snapshot->TagPosition();
		d_roi = QRect(tagPosition.x() - d_roiSize/2,
		              tagPosition.y() - d_roiSize/2,
		              d_roiSize,
		              d_roiSize);
		setSceneRect(QRect(0,0,d_image.width(),d_image.height()));
	}

	d_pixmap.convertFromImage(d_image.copy(d_roi),Qt::ColorOnly);

	if ( d_background != NULL ) {
		d_scene.removeItem(d_background);
		delete d_background;
	}

	d_background = new BackgroundPixmap(d_pixmap,*this);
	d_scene.addItem(d_background);

	d_background->setZValue(0);
	setImageCorner();

}


void SnapshotViewer::setImageCorner() {
	if ( !d_snapshot || d_snapshot->Corners().size() != 4) {
		for(size_t i = 0; i < 4; ++i) {
			d_tagCorners[i]->setVisible(false);
			d_tagLines[i]->setVisible(false);
		}
		return;
	}

	for(size_t i = 0; i < 4; ++i ) {

		auto & current = d_snapshot->Corners()[i];
		auto & next = d_snapshot->Corners()[(i+1)%4];

		const static size_t CORNER_OFFSET = (TAG_CORNER_POINT_SIZE-1)/2+1;

		d_tagCorners[i]->setRect(current.x()-CORNER_OFFSET - d_roi.x(),
		                         current.y()-CORNER_OFFSET - d_roi.y(),
		                         TAG_CORNER_POINT_SIZE+1,
		                         TAG_CORNER_POINT_SIZE+1);
		d_tagCorners[i]->setVisible(true);


		d_tagLines[i]->setLine(current.x()-d_roi.x(),
		                       current.y()-d_roi.y(),
		                       next.x()-d_roi.x(),
		                       next.y()-d_roi.y());
		d_tagLines[i]->setVisible(true);

	}

}

SnapshotViewer::BackgroundPixmap::BackgroundPixmap(const QPixmap & pixmap,SnapshotViewer & viewer,QGraphicsItem * parent)
	: QGraphicsPixmapItem(pixmap,parent)
	, d_viewer(viewer) {

}

SnapshotViewer::BackgroundPixmap::~BackgroundPixmap() {}


void SnapshotViewer::BackgroundPixmap::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	if ( d_viewer.d_poseEstimate ) {
		return;
	}

	d_viewer.d_head->setVisible(true);
	d_viewer.d_head->setEnabled(true);
	d_viewer.d_head->setPos(e->scenePos());

	d_viewer.d_tail->setVisible(true);
	d_viewer.d_tail->setEnabled(true);
	d_viewer.d_tail->setPos(e->scenePos());

	d_viewer.d_estimateOrig = std::make_shared<QPointF>(e->scenePos());
}

void SnapshotViewer::BackgroundPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_viewer.d_estimateOrig ) {
		return;
	}

	d_viewer.d_tail->setPos(e->scenePos());

	d_viewer.updateLine();
	//todo finish line drawing
}

void SnapshotViewer::BackgroundPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	d_viewer.d_estimateOrig.reset();
	d_viewer.emitNewPoseEstimate();
}



void SnapshotViewer::updateLine() {

	Eigen::Vector2d head(d_head->pos().x(),d_head->pos().y());
	Eigen::Vector2d tail(d_tail->pos().x(),d_tail->pos().y());
	Eigen::Vector2d diff = head - tail;
	double angle = 180.0 / M_PI * std::atan2(diff.y(),diff.x());
	d_tail->setRotation(angle);
	d_head->setRotation(angle);


	if (diff.norm() < (PositionMarker::MARKER_SIZE * 2) ) {
		d_estimateLine->setVisible(false);
		return;
	}

	diff.normalize();
	d_estimateLine->setVisible(true);
	Eigen::Vector2d start = head - (PositionMarker::MARKER_SIZE * diff);
	Eigen::Vector2d end = tail + (PositionMarker::MARKER_SIZE * diff);

	d_estimateLine->setLine(start.x(),start.y(),end.x(),end.y());



}

const  QColor SnapshotViewer::PositionMarker::COLOR = QColor(10,150,255,150);
const int SnapshotViewer::PositionMarker::MARKER_SIZE = 8;
SnapshotViewer::PositionMarker::PositionMarker(qreal x, qreal y,
                                               SnapshotViewer & viewer,
                                               QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_viewer(viewer) {

	setPos(x,y);
	setFlags(QGraphicsItem::ItemIsMovable);

	QPen markerPen(COLOR);
	markerPen.setWidth(3);
	QBrush markerBrush(QColor(10,150,255,40));

	QPainterPath path;
	path.moveTo(MARKER_SIZE,0);
	path.lineTo(0,-MARKER_SIZE);
	path.arcTo(QRect(-MARKER_SIZE,-MARKER_SIZE,2*MARKER_SIZE,2*MARKER_SIZE),90.0,180.0);
	path.closeSubpath();

	auto pathItem = new QGraphicsPathItem(path,this);
	pathItem->setPen(markerPen);
	pathItem->setBrush(markerBrush);


	auto center = new QGraphicsEllipseItem(QRect(-2,-2,4,4),this);
	QPen centerPen(COLOR);
	centerPen.setWidth(2);
	center->setPen(centerPen);


	QPen highlightPen(QColor(255,255,255));
	highlightPen.setWidth(1);

	auto pathHighlight = new QGraphicsPathItem(path,this);
	pathHighlight->setPen(highlightPen);


}

SnapshotViewer::PositionMarker::~PositionMarker() {
}


void SnapshotViewer::PositionMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseReleaseEvent(e);
	d_viewer.emitNewPoseEstimate();

}

void SnapshotViewer::PositionMarker::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseMoveEvent(e);
	//todo update poseEstimate
	d_viewer.updateLine();
}


void SnapshotViewer::setAntPoseEstimate(const AntPoseEstimate::Ptr & estimate) {
	if ( !d_snapshot ||  estimate->Path() != d_snapshot->Path() ) {
		return;
	}

	d_poseEstimate = estimate;
	if (!d_poseEstimate) {
		d_head->setVisible(false);
		d_tail->setVisible(true);
	}
	d_head->setPos(estimate->Head().x(),
	               estimate->Head().y());
	d_tail->setPos(estimate->Tail().x(),
	               estimate->Tail().y());
	d_head->setVisible(true);
	d_tail->setVisible(true);

}


void SnapshotViewer::emitNewPoseEstimate() {
	if (!d_snapshot) {
		return;
	}
	Eigen::Vector2d head(d_head->pos().x(),
	                     d_head->pos().y());
	Eigen::Vector2d tail(d_tail->pos().x(),
	                     d_tail->pos().y());

	if (!d_poseEstimate) {
		d_poseEstimate = std::make_shared<AntPoseEstimate>(head,
		                                                   tail,
		                                                   d_snapshot->Frame(),
		                                                   d_snapshot->TagValue());
	} else {
		d_poseEstimate->SetHead(head);
		d_poseEstimate->SetHead(tail);

	}

	emit antPoseEstimateUpdated(d_poseEstimate);

}
