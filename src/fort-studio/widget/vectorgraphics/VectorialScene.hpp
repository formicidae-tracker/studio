#pragma once

#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include <functional>

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

	const QVector<Vector*> & vectors() const;
	const QVector<Capsule*> & capsules() const;
	const QVector<Polygon*> & polygons() const;
	const QVector<Circle*> & circles() const;

	Circle * appendCircle(const QPointF & center, qreal radius);
	Capsule * appendCapsule(const QPointF & c1, const QPointF & c2,
	                        qreal r1, qreal r2);
	Polygon * appendPolygon(const QVector<QPointF> & vertices);
	Vector * appendVector(const QPointF & start, const QPointF & end);

	void setPoseIndicator(const QPointF & center, double angle);
	void clearPoseIndicator();

	void setBackgroundPicture(const QString & path);

public slots:
	void onZoomed(double factor);

	void setOnce(bool once);

	void setMode(Mode mode);

	void setColor(const QColor & color);

	void setHandleScaleFactor(double factor);

	void deleteShape(Shape * shape);
signals:
	void handleScaleFactorChanged(double factor);
	void modeChanged(Mode mode);
	void onceChanged(bool once);

	void colorChanged(const QColor & color);

	void vectorCreated(Vector * vector);

	void capsuleCreated(Capsule * capsule);

	void polygonCreated(Polygon * polygon);

	void circleCreated(Circle * circle);

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



	std::vector<Shape*> d_shapes;
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

	QVector<Vector*>  d_vectors;
	QVector<Capsule*> d_capsules;
	QVector<Polygon*> d_polygons;
	QVector<Circle*>  d_circles;

	PoseIndicator * d_poseIndicator;
	QGraphicsPixmapItem * d_background;
};

QDebug operator<<(QDebug, VectorialScene::Mode);
