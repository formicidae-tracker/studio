#include "SnapshotViewer.hpp"

#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

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
	//todo start line drawing

	if ( d_viewer.d_head->isVisible() == false ) {
		d_viewer.d_head->setVisible(true);
		d_viewer.d_head->setEnabled(true);
		d_viewer.d_head->setPos(e->scenePos());
		return;
	}
	if ( d_viewer.d_tail->isVisible() == false ) {
		d_viewer.d_tail->setVisible(true);
		d_viewer.d_tail->setEnabled(true);
		d_viewer.d_tail->setPos(e->scenePos());
		return;
	}
}

void SnapshotViewer::BackgroundPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	//todo finish line drawing
}

void SnapshotViewer::BackgroundPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	//todo update line drawing
}


SnapshotViewer::PositionMarker::PositionMarker(qreal x, qreal y,
                                               SnapshotViewer & viewer,
                                               QGraphicsItem * parent)
	: QGraphicsEllipseItem(QRect(-(MARKER_SIZE-1)/2,
	                             -(MARKER_SIZE-1)/2,
	                             MARKER_SIZE,
	                             MARKER_SIZE),
	                       parent)
	, d_viewer(viewer) {
	setPos(x,y);
	setFlags(QGraphicsItem::ItemIsMovable);

	static QPen markerPen(QColor(10,150,255,150));
	markerPen.setWidth(2);
	static QBrush markerBrush(QColor(10,150,255,40));
	setPen(markerPen);
	setBrush(markerBrush);

	auto center = new QGraphicsEllipseItem(QRect(0,0,1,1),this);
	center->setPen(markerPen);

}

SnapshotViewer::PositionMarker::~PositionMarker() {
}


void SnapshotViewer::PositionMarker::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsEllipseItem::mousePressEvent(e);
}

void SnapshotViewer::PositionMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsEllipseItem::mouseReleaseEvent(e);
	//todo update line

}

void SnapshotViewer::PositionMarker::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsEllipseItem::mouseMoveEvent(e);
	//todo update poseEstimate
}
