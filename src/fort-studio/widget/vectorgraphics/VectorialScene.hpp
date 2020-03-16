#pragma once

#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QSharedPointer>

#include <functional>
#include <myrmidon/priv/Types.hpp>



namespace fmp = fort::myrmidon::priv;


class Handle;
class Vector;
class Capsule;
class Polygon;
class Circle;
class Shape;

class VectorialScene : public QGraphicsScene {
	Q_OBJECT
	Q_PROPERTY(QColor color
	           READ color
	           WRITE setColor
	           NOTIFY colorChanged)
	Q_PROPERTY(Mode mode
	           READ mode
	           WRITE setMode
	           NOTIFY modeChanged)
	Q_PROPERTY(bool once
	           READ once
	           WRITE setOnce
	           NOTIFY onceChanged);
public:
	explicit VectorialScene(QObject * parent = nullptr);
	virtual ~VectorialScene();

	enum class Mode {
		Edit          = 0,
		InsertCapsule = 1,
		InsertCircle  = 2,
		InsertPolygon = 3,
		InsertVector  = 4,
	};

	const QColor color() const;
	Mode mode() const;
	bool once() const;

	double handleScaleFactor() const;

	const QVector<QSharedPointer<Vector>> & vectors() const;
	const QVector<QSharedPointer<Capsule>> & capsules() const;
	const QVector<QSharedPointer<Polygon>> & polygons() const;
	const QVector<QSharedPointer<Circle>> & circles() const;

	QSharedPointer<Circle>  appendCircle(const QPointF & center, qreal radius);
	QSharedPointer<Capsule> appendCapsule(const QPointF & c1, const QPointF & c2,
	                                      qreal r1, qreal r2);
	QSharedPointer<Polygon> appendPolygon(const QVector<QPointF> & vertices);
	QSharedPointer<Vector>  appendVector(const QPointF & start, const QPointF & end);


	void clearCircles();
	void clearCapsules();
	void clearVectors();
	void clearPolygons();

	void setPoseIndicator(const QPointF & center, double angle);
	void clearPoseIndicator();

	void setBackgroundPicture(const QString & path);


	void setStaticPolygon(const fmp::Vector2dList & corners,
	                      const QColor & color);
	void clearStaticPolygon();

public slots:
	void onZoomed(double factor);

	void setOnce(bool once);

	void setMode(Mode mode);

	void setColor(const QColor & color);

	void setHandleScaleFactor(double factor);

	void deleteShape(QSharedPointer<Shape> shape);
signals:
	void handleScaleFactorChanged(double factor);
	void modeChanged(Mode mode);
	void onceChanged(bool once);

	void colorChanged(const QColor & color);

	void vectorCreated(QSharedPointer<Vector> vector);
	void vectorRemoved(QSharedPointer<Vector> vector);

	void circleCreated(QSharedPointer<Circle> circle);
	void circleRemoved(QSharedPointer<Circle> circle);

	void capsuleCreated(QSharedPointer<Capsule> capsule);
	void capsuleRemoved(QSharedPointer<Capsule> capsule);

	void polygonCreated(QSharedPointer<Polygon> polygon);
	void polygonRemoved(QSharedPointer<Polygon> polygon);




protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void keyPressEvent(QKeyEvent * e) override;

private:
	class PoseIndicator : public QGraphicsItemGroup {
	public:
		PoseIndicator(QGraphicsItem * parent = nullptr);
		virtual ~PoseIndicator();
	};

	typedef std::function<void(QGraphicsSceneMouseEvent *mouseEvent)> EventHandler;

	void deleteShapePtr(Shape* shape);

	Mode                d_mode;
	QColor              d_color;
	bool                d_once;
	double              d_handleScaleFactor;

	EventHandler d_mousePress;
	EventHandler d_mouseMove;
	EventHandler d_mouseRelease;
	EventHandler d_editPressEH;
	EventHandler d_editMoveEH;
	EventHandler d_editReleaseEH;
	EventHandler d_insertVectorPressEH;
	EventHandler d_insertCapsulePressEH;
	EventHandler d_insertCirclePressEH;
	EventHandler d_insertPolygonPressEH;

	QVector<QSharedPointer<Vector>>  d_vectors;
	QVector<QSharedPointer<Capsule>> d_capsules;
	QVector<QSharedPointer<Polygon>> d_polygons;
	QVector<QSharedPointer<Circle>>  d_circles;

	PoseIndicator        * d_poseIndicator;
	QGraphicsPixmapItem  * d_background;
	QGraphicsPolygonItem * d_staticPolygon;
};

QDebug operator<<(QDebug, VectorialScene::Mode);
