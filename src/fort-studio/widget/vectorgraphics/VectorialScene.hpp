#pragma once

#include <QGraphicsScene>

class Handle;
class Vector;
class Capsule;
class Polygon;
class Circle;


class VectorialScene : public QGraphicsScene {
	Q_OBJECT
public:
	explicit VectorialScene(QObject * parent = nullptr);
	virtual ~VectorialScene();

	Vector  * d_vector;
	Capsule * d_capsule;
	Polygon * d_polygon;
	Circle  * d_circle;
};
