#include "VectorialScene.hpp"

#include "Handle.hpp"
#include "Vector.hpp"
#include <QDebug>

#include <fort-studio/widget/base/ColorComboBox.hpp>



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

	d_vector = new Vector(100,100,300,200,
	                      ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(0)),
	                      [](const QPointF & start, const QPointF & end) {
		                      qDebug() << "Now " << start << " --> " << end;
	                      },
	                      this);
	addItem(d_vector);
}

VectorialScene::~VectorialScene() {
}
