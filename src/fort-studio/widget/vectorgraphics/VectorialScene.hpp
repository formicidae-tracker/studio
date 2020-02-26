#pragma once

#include <QGraphicsScene>
#include <functional>

class Handle;
class Vector;
class Capsule;
class Polygon;
class Circle;
class Shape;

class VectorialScene : public QGraphicsScene {
	Q_OBJECT
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

	void setColor(const QColor & color);

	void setMode(Mode mode);

signals:

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;


private:
	typedef std::function<void(QGraphicsSceneMouseEvent *mouseEvent)> EventHandler;



	std::vector<Shape*> d_shapes;
	Mode                d_mode;
	QColor              d_color;
	bool                d_once;

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
};
