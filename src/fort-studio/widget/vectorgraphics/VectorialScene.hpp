#pragma once

#include <QGraphicsScene>

class Handle;
class Vector;
class Capsule;
class Polygon;
class VectorialScene : public QGraphicsScene {
	Q_OBJECT
public:
	explicit VectorialScene(QObject * parent = nullptr);
	virtual ~VectorialScene();

	Handle * d_handles[4];
	Vector * d_vector;
	Capsule * d_capsule;
	Polygon * d_polygon;
};
