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
	void setAntPoseEstimate(const AntPoseEstimate::Ptr & s);

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

	class PoseMarker : public QGraphicsItemGroup {
	public:
		PoseMarker(QGraphicsItem * parent = NULL);
		virtual ~PoseMarker();

		const static QColor OUTSIDE_COLOR;
		const static QColor INSIDE_COLOR;
		const static int    SIZE;
	};

	class Handle : public QGraphicsItemGroup {
	public:
		const static int SIZE;
		const static QColor COLOR;

		Handle(QGraphicsItem * parent);
		virtual ~Handle();

	private:
		QGraphicsRectItem * d_inside;

	};


	class Capsule : public QGraphicsItemGroup {
	public :
		const static QColor COLOR_BORDER,COLOR_INSIDE;
		Capsule(qreal c1x,qreal c1y,qreal r1,
		        qreal c2x, qreal c2y, qreal r2,
		        QGraphicsItem *parent = NULL );
		virtual ~Capsule();
	private:
		void Rebuild();

		QGraphicsPathItem * d_path;
		Handle * d_c1;
		Handle * d_c2;
		double d_r1,d_r2;


		Handle * d_r1Handle;
		Handle * d_r2Handle;
	};

	const static int DEFAULT_ROI_SIZE;
	const static int TAG_LINE_SIZE;
	const static QColor TAG_LINE_COLOR;

	void setImageBackground();
	void setTagCorner();

	void updateLine();
	void emitNewPoseEstimate();

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

	Handle            * d_tagCorners[4];
	QGraphicsLineItem * d_tagLines[4];

	AntPoseEstimate::Ptr     d_poseEstimate;
	PositionMarker         * d_head, * d_tail;
	QGraphicsLineItem      * d_estimateLine;
	std::shared_ptr<QPointF> d_estimateOrig;

	PoseMarker * d_poseMarker;


	Capsule * d_capsule;

};
