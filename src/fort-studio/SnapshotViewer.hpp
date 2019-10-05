#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemGroup>

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
	class BackgroundPixmap : public QGraphicsPixmapItem {
	public:
		BackgroundPixmap(const QPixmap & pixmap,
		                 SnapshotViewer & viewer,
		                 QGraphicsItem * parent = NULL);
		virtual ~BackgroundPixmap();

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent * e) override;
		void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;
	private :
		SnapshotViewer & d_viewer;
	};


	class PositionMarker : public QGraphicsItemGroup {
	public:
		PositionMarker(qreal x, qreal y,
		               SnapshotViewer & viewer,
		               QGraphicsItem * parent = NULL);
		virtual ~PositionMarker();
		const static int MARKER_SIZE;
		const static QColor COLOR;
	protected:
		void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;

	private:

		SnapshotViewer & d_viewer;
	};

	const static int TAG_CORNER_POINT_SIZE = 3;
	const static int TAG_LINE_SIZE = 2;

	void setImageBackground();
	void setImageCorner();

	void updateLine();

	QGraphicsScene     d_scene;

	std::filesystem::path d_basedir;

	size_t             d_roiSize;
	QRect              d_roi;


	Snapshot::ConstPtr    d_snapshot;
	QImage                d_image;
	QPixmap               d_pixmap;
	BackgroundPixmap *    d_background;
	QPen                  d_tagCornerPen,d_tagLinePen;
	QBrush                d_tagCornerBrush;

	QGraphicsRectItem *   d_tagCorners[4];
	QGraphicsLineItem *   d_tagLines[4];

	AntPoseEstimate::Ptr  d_poseEstimate;
	PositionMarker      * d_head, * d_tail;
	QGraphicsLineItem   * d_estimateLine;
	std::shared_ptr<QPointF> d_estimateOrig;
};
