#include "SnapshotViewer.hpp"

#include <QGraphicsPixmapItem>

SnapshotViewer::SnapshotViewer(QWidget *parent)
	: QGraphicsView(parent)
	, d_roiSize(400)
	, d_pixmapItem(NULL) {
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

	if ( d_pixmapItem != NULL ) {
		d_scene.removeItem(d_pixmapItem);
		delete d_pixmapItem;
	}

	d_pixmapItem = d_scene.addPixmap(d_pixmap);

	d_pixmapItem->setEnabled(false);
	d_pixmapItem->setZValue(0);
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
