#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>


#include "Snapshot.hpp"
#include "AntPoseEstimate.hpp"

class SnapshotViewer : public QGraphicsView {
    Q_OBJECT
	Q_PROPERTY(size_t roiSize
	           READ roiSize
	           WRITE setRoiSize
	           NOTIFY roiSizeChanged);
public:
	explicit SnapshotViewer(QWidget *parent = 0);
    virtual ~SnapshotViewer();

	void setBasedir(const std::filesystem::path & basedir);

	size_t roiSize() const;

public slots:
	void displaySnapshot(const Snapshot::ConstPtr & s);
	void setRoiSize(size_t);


signals:
	void antPoseEstimateUpdated(const AntPoseEstimate::Ptr & estimate);
	void roiSizeChanged(size_t);

private:
	void setImageBackground();
	void setImageCorner();

	QGraphicsScene     d_scene;

	std::filesystem::path d_basedir;

	size_t             d_roiSize;
	QRect              d_roi;


	Snapshot::ConstPtr    d_snapshot;
	QImage                d_image;
	QPixmap               d_pixmap;
	QGraphicsPixmapItem * d_pixmapItem;
	QPen                  d_tagCornerPen,d_tagLinePen;
	QBrush                d_tagCornerBrush;

	QGraphicsRectItem *   d_tagCorners[4];
	QGraphicsLineItem *   d_tagLines[4];


	const static size_t TAG_CORNER_POINT_SIZE = 3;
	const static size_t TAG_LINE_SIZE = 2;
};
