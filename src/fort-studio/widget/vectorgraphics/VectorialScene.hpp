#pragma once

#include <QGraphicsScene>

class Handle;

class VectorialScene : public QGraphicsScene {
	Q_OBJECT
public:
	explicit VectorialScene(QObject * parent = nullptr);
	virtual ~VectorialScene();

	Handle * d_handles[4];
};
