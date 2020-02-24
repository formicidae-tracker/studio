#include "VectorialScene.hpp"

#include "Handle.hpp"
#include "Vector.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"


#include <iostream>

#include <fort-studio/widget/base/ColorComboBox.hpp>

#include <myrmidon/priv/Capsule.hpp>

VectorialScene::VectorialScene(QObject * parent)
	: QGraphicsScene(parent) {


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


	d_polygon = new Polygon({{0,200},{200,200},{300,100},{0,0},{0,200}},
	                        ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(2)),
	                        [](const QVector<QPointF> & points) {
		                        std::cerr << "updated polygon {";
		                        std::string prefix = "";
		                        for ( const auto & p : points ) {
			                        std::cerr << prefix << "(" << p.x()
			                                  << "," << p.y() << ")";
			                        prefix = ",";
		                        }
		                        std::cerr << std::endl;
	                        },
	                        NULL);
	d_polygon->addToScene(this);

	d_circle = new Circle({600,600},60,
	                      ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(3)));

	d_circle->addToScene(this);
	connect(d_circle,
	        &Shape::updated,
	        [this]() {
		        auto p = d_circle->pos();
		        std::cerr << "Updated circle center:(" << p.x() << ","
		                  << p.y() << ") radius:"
		                  << d_circle->radius()
		                  << std::endl;
	        });


}

VectorialScene::~VectorialScene() {
}
