#include "color.h"


#include <Rcpp.h>


void fmColor_show(const fort::myrmidon::Color * c) {
	Rcpp::Rcout << "fmColor ( R = " << (int)std::get<0>(*c)
	            << ", G = " << (int)std::get<1>(*c)
	            << ", B = " << (int)std::get<2>(*c)
	            << ")\n";
}

int fmColor_red(const fort::myrmidon::Color * c) {
	return std::get<0>(*c);
}

int fmColor_green(const fort::myrmidon::Color * c) {
	return std::get<1>(*c);
}

int fmColor_blue(const fort::myrmidon::Color * c) {
	return std::get<2>(*c);
}

fort::myrmidon::Color * fmRGBColor(SEXP xx) {
	Rcpp::IntegerVector v(xx);
	if ( v.size() != 3 ) {
		throw std::runtime_error("fmRGBColor needs an integer vector of size 3");
	}
	return new fort::myrmidon::Color(std::clamp(v[0],0,255),
	                                 std::clamp(v[1],0,255),
	                                 std::clamp(v[2],0,255));
}

fort::myrmidon::Color * fmDefaultPaletteColor(size_t i) {
	return new fort::myrmidon::Color(fort::myrmidon::Palette::Default().At(i));
}

RCPP_MODULE(color) {
	Rcpp::class_<fort::myrmidon::Color>("fmColor")
		.const_method("show",&fmColor_show)
		.const_method("red",&fmColor_red)
		.const_method("green",&fmColor_green)
		.const_method("blue",&fmColor_blue)
		;

	Rcpp::function("fmRGBColor",&fmRGBColor);
	Rcpp::function("fmDefaultPaletteColor",&fmDefaultPaletteColor);



}
