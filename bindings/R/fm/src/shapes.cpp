#include "shapes.h"

#include "eigen.h"

void fmCircle_show(const fort::myrmidon::Circle * c) {
	Rcpp::Rcout << "Circle ( center = "
	            << c->Center().transpose()
	            << ", radius = "
	            << c->Radius()
	            << " )\n";
}

fort::myrmidon::Circle * fmCircle_create(const Eigen::Vector2d & center, double radius) {
	return new fort::myrmidon::Circle(center,radius);
}

void fmCapsule_show(const fort::myrmidon::Capsule * c) {
	Rcpp::Rcout << "Capsule ( center1 = "
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



RCPP_MODULE(shapes) {
	Rcpp::class_<fort::myrmidon::Shape>("fmShape");

	Rcpp::class_<fort::myrmidon::Circle>("fmCircle")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<Eigen::Vector2d,double>()
		.const_method("show",&fmCircle_show)
		;

	Rcpp::class_<fort::myrmidon::Capsule>("fmCapsule")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<Eigen::Vector2d,Eigen::Vector2d,double,double>()
		.const_method("show",&fmCapsule_show)
		;


	Rcpp::function("fmCircleCreate", &fmCircle_create);
	Rcpp::function("fmCapsuleCreate", &fmCapsule_create);

}
