#include "shapes.h"

#include "eigen.h"

void fmCircle_show(const fort::myrmidon::Circle * c) {
	Rcpp::Rcout << "fmCircle ( center = "
	            << c->Center().transpose()
	            << ", radius = "
	            << c->Radius()
	            << " )\n";
}

fort::myrmidon::Circle * fmCircle_create(const Eigen::Vector2d & center, double radius) {
	return new fort::myrmidon::Circle(center,radius);
}

void fmCapsule_show(const fort::myrmidon::Capsule * c) {
	Rcpp::Rcout << "fmCapsule ( center1 = "
	            << c->C1().transpose()
	            << ", center2 = "
	            << c->C2().transpose()
	            << ", radius1 = "
	            << c->R1()
	            << ", radius2 = "
	            << c->R2()
	            << " )\n";
}

fort::myrmidon::Capsule * fmCapsule_create(const Eigen::Vector2d & c1,
                                           const Eigen::Vector2d & c2,
                                           double r1,
                                           double r2) {
	return new fort::myrmidon::Capsule(c1,c2,r1,r2);
}

void fmPolygon_show(const fort::myrmidon::Polygon * p) {
	Rcpp::Rcout << "fmPolygon (";
	for ( size_t i = 0 ; i < p->Size(); ++i ) {
		Rcpp::Rcout <<" (" << p->Vertex(i).transpose() << ")";
	}
	Rcpp::Rcout << " )\n";
}


fort::myrmidon::Polygon * fmPolygon_create(const fort::myrmidon::Vector2dList & vertices) {
	return new fort::myrmidon::Polygon(vertices);
}


RCPP_MODULE(shapes) {
	Rcpp::class_<fort::myrmidon::Shape>("fmShape");

	Rcpp::class_<fort::myrmidon::Circle>("fmCircle")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<Eigen::Vector2d,double>()
		.const_method("show",&fmCircle_show)
		.const_method("center",&fort::myrmidon::Circle::Center)
		.const_method("radius",&fort::myrmidon::Circle::Radius)
		.method("setCenter",&fort::myrmidon::Circle::SetCenter)
		.method("setRadius",&fort::myrmidon::Circle::SetRadius)
		;

	Rcpp::class_<fort::myrmidon::Capsule>("fmCapsule")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<Eigen::Vector2d,Eigen::Vector2d,double,double>()
		.const_method("show",&fmCapsule_show)
		.const_method("c1",&fort::myrmidon::Capsule::C1)
		.const_method("r1",&fort::myrmidon::Capsule::R1)
		.const_method("c2",&fort::myrmidon::Capsule::C2)
		.const_method("r2",&fort::myrmidon::Capsule::R2)
		.method("setC1",&fort::myrmidon::Capsule::SetC1)
		.method("setR1",&fort::myrmidon::Capsule::SetR1)
		.method("setC2",&fort::myrmidon::Capsule::SetC2)
		.method("setR2",&fort::myrmidon::Capsule::SetR2)
		;

	Rcpp::class_<fort::myrmidon::Polygon>("fmPolygon")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<fort::myrmidon::Vector2dList>()
		.const_method("show",&fmPolygon_show)
		.const_method("size",&fort::myrmidon::Polygon::Size)
		.const_method("vertex",&fort::myrmidon::Polygon::Vertex)
		.method("setVertex",&fort::myrmidon::Polygon::SetVertex)
		;


	Rcpp::function("fmCircleCreate", &fmCircle_create);
	Rcpp::function("fmCapsuleCreate", &fmCapsule_create);
	Rcpp::function("fmPolygonCreate", &fmPolygon_create);

}
