#include "VectorialScene.hpp"

#include "Handle.hpp"
#include <QDebug>

VectorialScene::VectorialScene(QObject * parent)
	: QGraphicsScene(parent) {

	for( size_t i = 0; i < 4; ++i) {
		d_handles[i] =
			new Handle([this,i]() {
				           qDebug() << i << " moved to " << d_handles[i]->pos();
			           },
				[this,i]() {
					qDebug() << i << " release at " << d_handles[i]->pos();
				});
		addItem(d_handles[i]);
	}
	d_handles[0]->setPos(QPointF(0,400));
	d_handles[1]->setPos(QPointF(400,400));
	d_handles[2]->setPos(QPointF(400,0));
	d_handles[3]->setPos(QPointF(0,0));

}

VectorialScene::~VectorialScene() {
}
