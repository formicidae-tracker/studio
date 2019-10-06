#include "SnapshotViewer.hpp"

#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QPainterPathStroker>
#include <QScrollBar>

#include <Eigen/Geometry>

const int SnapshotViewer::DEFAULT_ROI_SIZE = 400;

const int SnapshotViewer::TAG_LINE_SIZE = 2;
const QColor SnapshotViewer::TAG_LINE_COLOR = QColor(255,20,20,120);

const int SnapshotViewer::Handle::SIZE = 5;
const QColor SnapshotViewer::Handle::COLOR = QColor(50,50,50,120);
const QColor SnapshotViewer::Handle::HIGHLIGHT_COLOR = QColor(150,150,150,120);

const QColor SnapshotViewer::PositionMarker::COLOR = QColor(10,150,255,150);
const int SnapshotViewer::PositionMarker::MARKER_SIZE = 8;

const QColor SnapshotViewer::PoseMarker::INSIDE_COLOR = QColor(255,255,255,180);
const QColor SnapshotViewer::PoseMarker::OUTSIDE_COLOR = QColor(0,0,0,180);
const int SnapshotViewer::PoseMarker::SIZE = 9;

const QColor SnapshotViewer::Capsule::COLOR_BORDER = QColor(150,10,255);
const QColor SnapshotViewer::Capsule::COLOR_INSIDE = QColor(150,10,255,40);

Eigen::Vector2d ToEigen(const QPointF & p) {
	return Eigen::Vector2d(p.x(),p.y());
}


Eigen::Vector2d ToEigen(QGraphicsItem * item) {
	return ToEigen(item->pos());
}


SnapshotViewer::SnapshotViewer(QWidget *parent)
	: QGraphicsView(parent)
	, d_roiSize(DEFAULT_ROI_SIZE)
	, d_background(NULL) {

	d_tagLinePen = QPen(TAG_LINE_COLOR);
	d_tagLinePen.setWidth(TAG_LINE_SIZE);

	for(size_t i = 0;i < 4; ++i ) {


		d_tagCorners[i] = new Handle();
		d_scene.addItem(d_tagCorners[i]);
		d_tagCorners[i]->setVisible(false);
		d_tagCorners[i]->setEnabled(false);

		d_tagCorners[i]->setZValue(2);
		d_tagLines[i] = d_scene.addLine(0,0,0,0,d_tagLinePen);
		d_tagLines[i]->setZValue(1);
		d_tagLines[i]->setVisible(false);
		d_tagLines[i]->setEnabled(false);

	}


	d_head = new PositionMarker(10.1,17.3,*this);
	d_tail = new PositionMarker(42.0,38.1,*this);
	d_head->setZValue(12);
	d_tail->setZValue(13);
	d_scene.addItem(d_head);
	d_scene.addItem(d_tail);
	d_head->setVisible(false);
	d_head->setEnabled(false);
	d_tail->setVisible(false);
	d_tail->setEnabled(false);

	d_estimateLine = new QGraphicsLineItem(0,0,0,0);
	d_estimateLine->setVisible(false);
	QPen linePen(PositionMarker::COLOR);
	linePen.setWidth(3);
	d_estimateLine->setPen(linePen);
	d_estimateLine->setZValue(11);
	d_scene.addItem(d_estimateLine);


	d_poseMarker = new PoseMarker();
	d_poseMarker->setVisible(false);
	d_poseMarker->setEnabled(false);

	d_poseMarker->setZValue(21);
	d_scene.addItem(d_poseMarker);


	setScene(&d_scene);
	setRoiSize(d_roiSize);
	displaySnapshot(Snapshot::ConstPtr());

}

SnapshotViewer::~SnapshotViewer() {
}


void SnapshotViewer::displaySnapshot(const Snapshot::ConstPtr & s) {
	d_snapshot = s;

	d_capsule.reset();
	if (!d_snapshot) {
		for(size_t i = 0; i < 4; ++i) {
			d_tagCorners[i]->setVisible(false);
			d_tagLines[i]->setVisible(false);
		}
		d_poseMarker->setVisible(false);
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
	setTagCorner();

	setSceneRect(QRect(0,0,d_roiSize,d_roiSize));
}


void SnapshotViewer::setTagCorner() {
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

		d_tagCorners[i]->setPos(current.x() - d_roi.x(),
		                        current.y() - d_roi.y());
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
	setAcceptHoverEvents(true);
}

SnapshotViewer::BackgroundPixmap::~BackgroundPixmap() {}


void SnapshotViewer::BackgroundPixmap::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_viewer.d_poseEstimate ) {
		d_viewer.d_head->setVisible(true);
		d_viewer.d_head->setEnabled(true);
		d_viewer.d_head->setPos(e->scenePos());

		d_viewer.d_tail->setVisible(true);
		d_viewer.d_tail->setEnabled(true);
		d_viewer.d_tail->setPos(e->scenePos());

		d_viewer.d_estimateOrig = std::make_shared<QPointF>(e->scenePos());
		return;
	}

	if ( !d_viewer.d_capsule ) {
		auto pos = ToEigen(e->scenePos());
		d_viewer.d_capsule = std::make_shared<Capsule>(pos.x(),pos.y(),0,
		                                               pos.x(),pos.y(),0,
		                                               &d_viewer.d_scene);
		d_viewer.d_capsule->setZValue(0);
		d_viewer.d_capsuleOrig = std::make_shared<QPointF>(e->scenePos());
	}
}

void SnapshotViewer::BackgroundPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	if ( d_viewer.d_estimateOrig ) {
		d_viewer.d_tail->setPos(e->scenePos());

		d_viewer.updateLine();
		return;
	}

	if ( d_viewer.d_capsuleOrig ) {
		d_viewer.d_capsule->d_c2->setPos(e->scenePos());
		auto c1(ToEigen(d_viewer.d_capsule->d_c1.get()));
		auto c2(ToEigen(d_viewer.d_capsule->d_c2.get()));
		double radius = std::min(30.0,(c2-c1).norm()/2);
		d_viewer.d_capsule->d_r1 = radius;
		d_viewer.d_capsule->d_r2 = radius;

		d_viewer.d_capsule->Rebuild();
	}



}

void SnapshotViewer::BackgroundPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	if ( d_viewer.d_estimateOrig ) {
		d_viewer.d_estimateOrig.reset();
		d_viewer.emitNewPoseEstimate();
		return;
	}

	if ( d_viewer.d_capsuleOrig ) {
		d_viewer.d_capsuleOrig.reset();
	}

}



void SnapshotViewer::updateLine() {

	Eigen::Vector2d head(ToEigen(d_head));
	Eigen::Vector2d tail(ToEigen(d_tail));
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
	if ( !estimate ) {
		d_poseEstimate.reset();
		d_head->setVisible(false);
		d_tail->setVisible(false);
		d_estimateLine->setVisible(false);
		return;
	}


	if ( !d_snapshot ||  estimate->Path() != d_snapshot->Path() ) {
		return;
	}

	d_poseEstimate = estimate;
	d_head->setPos(estimate->Head().x(),
	               estimate->Head().y());
	d_tail->setPos(estimate->Tail().x(),
	               estimate->Tail().y());
	d_head->setVisible(true);
	d_tail->setVisible(true);
	updateLine();
}


void SnapshotViewer::emitNewPoseEstimate() {
	if (!d_snapshot) {
		return;
	}
	Eigen::Vector2d head(ToEigen(d_head));
	Eigen::Vector2d tail(ToEigen(d_tail));

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


SnapshotViewer::PoseMarker::PoseMarker(QGraphicsItem * parent)
	: QGraphicsItemGroup(parent) {
	static int HALF_SIZE =(SIZE-1)/2;

	QPen empty;
	empty.setWidth(0);
	QPen outside = QPen(OUTSIDE_COLOR);
	outside.setWidth(4);
	QPen inside = QPen(INSIDE_COLOR);
	inside.setWidth(1);

	QPainterPath basepath;

	basepath.moveTo(HALF_SIZE,0.5);
	basepath.lineTo(3*SIZE,0.5);
	basepath.lineTo(2*SIZE + HALF_SIZE,HALF_SIZE+0.5);
	basepath.moveTo(2*SIZE + HALF_SIZE,-HALF_SIZE+0.5);
	basepath.lineTo(3*SIZE,0.5);

	QPainterPathStroker stroker;
	stroker.setCapStyle(Qt::RoundCap);
	QPainterPath path = stroker.createStroke(basepath);

	auto outsidePath = new QGraphicsPathItem(path,this);
	outsidePath->setPen(outside);
	auto insidePath = new QGraphicsPathItem(path,this);
	insidePath->setPen(inside);



	auto centerOutside = new QGraphicsEllipseItem(QRect(-HALF_SIZE-1,
	                                                    -HALF_SIZE-1,
	                                                    SIZE+2,
	                                                    SIZE+2),
	                                              this);
	auto centerInside = new QGraphicsEllipseItem(QRect(-HALF_SIZE,
	                                                   -HALF_SIZE,
	                                                   SIZE,
	                                                   SIZE),
	                                             this);

	auto center = new QGraphicsEllipseItem(QRect(-1,-1,3,3),this);

	centerOutside->setPen(empty);
	centerOutside->setBrush(outside.brush());
	centerInside->setPen(empty);
	centerInside->setBrush(inside.brush());
	center->setPen(empty);
	center->setBrush(outside.brush());


}

SnapshotViewer::PoseMarker::~PoseMarker() {}


SnapshotViewer::Handle::Handle(SnapshotViewer::Handle::MoveCallback onMove,
                               QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onMove(onMove) {
	const static int HALF_SIZE = (SIZE-1)/2;
	QPen empty;
	empty.setStyle(Qt::NoPen);

	d_inside = new QGraphicsRectItem(-HALF_SIZE-0.5,
	                                 -HALF_SIZE-0.5,
	                                 SIZE,
	                                 SIZE,this);
	d_inside->setPen(QPen(QColor(255,255,255),1));
	d_inside->setBrush(COLOR);
	setFlags(QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents(true);
}

SnapshotViewer::Handle::~Handle() {}


void SnapshotViewer::Handle::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItem::mouseMoveEvent(e);
	d_onMove(e->scenePos());
}



SnapshotViewer::Capsule::Capsule(qreal c1x,qreal c1y,qreal r1,
                                 qreal c2x, qreal c2y, qreal r2,
                                 QGraphicsScene * parent)
	: d_r1(r1)
	, d_r2(r2)
	, d_path(std::make_shared<QGraphicsPathItem>())
	, d_parent(parent) {

	d_c1 = std::make_shared<Handle>([this](const QPointF &) { Rebuild(); });
	d_c2 = std::make_shared<Handle>([this](const QPointF &) { Rebuild(); });
	d_c1->setPos(c1x,c1y);
	d_c2->setPos(c2x,c2y);
	d_r1Handle = std::make_shared<Handle>([this](const QPointF & pos) {
		                                      d_r1 = (ToEigen(d_c1.get()) - ToEigen(pos)).norm();
		                                      Rebuild();
	                                      });
	d_r2Handle = std::make_shared<Handle>([this](const QPointF & pos) {
		                                      d_r2 = (ToEigen(d_c2.get()) - ToEigen(pos)).norm();
		                                      Rebuild();
	                                      });

	d_path->setPen(QPen(COLOR_BORDER,2));
	d_path->setBrush(COLOR_INSIDE);

	Rebuild();

	d_parent->addItem(d_path.get());
	d_parent->addItem(d_c1.get());
	d_parent->addItem(d_c2.get());
	d_parent->addItem(d_r1Handle.get());
	d_parent->addItem(d_r2Handle.get());
}

SnapshotViewer::Capsule::~Capsule() {
	d_parent->removeItem(d_r2Handle.get());
	d_parent->removeItem(d_r1Handle.get());
	d_parent->removeItem(d_c2.get());
	d_parent->removeItem(d_c1.get());
	d_parent->removeItem(d_path.get());
}

void SnapshotViewer::Capsule::setZValue(int z) {
	d_path->setZValue(30 + 10*z + 0);
	d_c1->setZValue(30 + 10*z + 1);
	d_c2->setZValue(30 + 10*z + 2);
	d_r1Handle->setZValue(30 + 10*z + 3);
	d_r2Handle->setZValue(30 + 10*z + 4);
}



void SnapshotViewer::Capsule::Rebuild() {
	Eigen::Vector2d c1(ToEigen(d_c1.get()));
	Eigen::Vector2d c2(ToEigen(d_c2.get()));
	Eigen::Vector2d diff = c2 - c1;
	double distance = diff.norm();

	if (d_r1 < 1e-6 || d_r2 < 1e-6 || distance < 1e-6) {
		return;
	}

	double angle = std::atan2(d_r2-d_r1,distance);

	diff /= distance;

	Eigen::Vector2d normal = Eigen::Rotation2D<double>(-angle) * Eigen::Vector2d(-diff.y(),diff.x());

	Eigen::Vector2d r1Pos = c1 - normal * d_r1;
	Eigen::Vector2d r2Pos = c2 - normal * d_r2;
	Eigen::Vector2d r2Opposite = c2 + Eigen::Rotation2D<double>(angle) * Eigen::Vector2d(-diff.y(),diff.x()) * d_r2;

	d_r1Handle->setPos(r1Pos.x(),r1Pos.y());
	d_r2Handle->setPos(r2Pos.x(),r2Pos.y());

	QPainterPath path;
	double startAngle1 = ( std::atan2(normal.y(),-normal.x()) ) * 180 / M_PI ;
	double startAngle2 = 180.0 + startAngle1 - 2 * angle  * 180 / M_PI;

	path.moveTo(r1Pos.x(),r1Pos.y());
	path.arcTo(QRect(c1.x() - d_r1,
	                 c1.y() - d_r1,
	                 2*d_r1,
	                 2*d_r1),
	           startAngle1,
	           180 - 2*angle * 180.0 / M_PI);
	path.lineTo(r2Opposite.x(),r2Opposite.y());
	path.arcTo(QRect(c2.x() - d_r2,
	                 c2.y() - d_r2,
	                 2 * d_r2,
	                 2 * d_r2),
	           startAngle2,
	           180 + 2*angle * 180.0 / M_PI);
	path.closeSubpath();
	d_path->setPath(path);
}
