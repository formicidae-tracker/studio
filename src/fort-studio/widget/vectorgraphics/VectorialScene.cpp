#include "VectorialScene.hpp"

#include "Handle.hpp"
#include "Vector.hpp"
#include "Capsule.hpp"
#include <iostream>

#include <fort-studio/widget/base/ColorComboBox.hpp>



VectorialScene::VectorialScene(QObject * parent)
	: QGraphicsScene(parent) {

	for( size_t i = 0; i < 4; ++i) {
		d_handles[i] =
			new Handle([this,i]() {
				           const auto & pos = d_handles[i]->pos();
				           std::cerr << i << " moved to ("
				                     << pos.x() << ","
				                     << pos.y() << ")"
				                     << std::endl;
			           },
				[this,i]() {
					const auto & pos = d_handles[i]->pos();
					std::cerr << i << " released at ("
					          << pos.x() << ","
					          << pos.y() << ")"
					          << std::endl;

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
		                      std::cerr << "Vector updated to ("
		                                << start.x() << "," << start.y() << ") --> ("
		                                << end.x() << "," << end.y() << ")"
		                                << std::endl;
	                      });
	addItem(d_vector);
	addItem(d_vector->startPrecisionHandle());
	addItem(d_vector->endPrecisionHandle());

	d_capsule = new Capsule(QPointF(50,50),QPointF(400,50),100,130,
	                        ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(1)),
	                        [](const QPointF & c1, const QPointF & c2, qreal r1, qreal r2) {
		                        std::cerr << "capsule update "
		                                  << fmp::Capsule(Eigen::Vector2d(c1.x(),c1.y()),
		                                                  Eigen::Vector2d(c2.x(),c2.y()),
		                                                  r1,r2)
		                                  << std::endl;
	                        });
	addItem(d_capsule);
	addItem(d_capsule->c1Handle());
	addItem(d_capsule->c2Handle());
	addItem(d_capsule->r1Handle());
	addItem(d_capsule->r2Handle());


}

VectorialScene::~VectorialScene() {
}
