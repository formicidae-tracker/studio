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
	                      ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(0)));
	d_vector->addToScene(this);
	connect(d_vector,
	        &Shape::updated,
	        [this]() {
		        auto start = d_vector->startPos();
		        auto end = d_vector->endPos();
		        std::cerr << "Vector update: ("
		                  << start.x() << "," << start.y() << ") -> ("
		                  << end.x() << "," << end.y() << ")"
		                  << std::endl;
	        });

	d_capsule = new Capsule(QPointF(50,50),QPointF(400,50),100,130,
	                        ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(1)));

	d_capsule->addToScene(this);

	connect(d_capsule,
	        &Shape::updated,
	        [this]() {
		        auto c1 = d_capsule->c1Pos();
		        auto c2 = d_capsule->c2Pos();
		        std::cerr << "capsule update "
		                  << fmp::Capsule(Eigen::Vector2d(c1.x(),c1.y()),
		                                  Eigen::Vector2d(c2.x(),c2.y()),
		                                  d_capsule->r1(),
		                                  d_capsule->r2())
		                  << std::endl;
	        });


	d_polygon = new Polygon({{0,200},{200,200},{300,100},{0,0},{0,200}},
	                        ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(2)));


	d_polygon->addToScene(this);

	connect(d_polygon,
	        &Shape::updated,
	        [this]() {
		        auto points = d_polygon->vertices();
		        std::cerr << "updated polygon {";
		        std::string prefix = "";
		        for ( const auto & p : points ) {
			        std::cerr << prefix << "(" << p.x()
			                  << "," << p.y() << ")";
			                        prefix = ",";
		        }
		        std::cerr << std::endl;
	        });

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
