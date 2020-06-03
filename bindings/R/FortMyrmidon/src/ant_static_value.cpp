#include "ant_static_value.h"

#include "time.h"


#include "Rcpp.h"

void fmAntStaticValue_show(const fort::myrmidon::AntStaticValue * v) {
	Rcpp::Rcout << "fmAntStaticValue " << *v << "\n";
}

bool fmAntStaticValue_toBool(const fort::myrmidon::AntStaticValue * v) {
	return std::get<bool>(*v);
}

fort::myrmidon::AntStaticValue fmAntStaticBool(bool v) {
	return fort::myrmidon::AntStaticValue(v);
}


int32_t fmAntStaticValue_toInteger(const fort::myrmidon::AntStaticValue * v) {
	return std::get<int32_t>(*v);
}

fort::myrmidon::AntStaticValue fmAntStaticInteger(int32_t v) {
	return fort::myrmidon::AntStaticValue(v);
}

double fmAntStaticValue_toNumeric(const fort::myrmidon::AntStaticValue * v) {
	return std::get<double>(*v);
}

fort::myrmidon::AntStaticValue fmAntStaticNumeric(double v) {
	return fort::myrmidon::AntStaticValue(v);
}

std::string fmAntStaticValue_toString(const fort::myrmidon::AntStaticValue * v) {
	return std::get<std::string>(*v);
}

fort::myrmidon::AntStaticValue fmAntStaticString(const std::string & v) {
	return fort::myrmidon::AntStaticValue(v);
}

fort::myrmidon::Time fmAntStaticValue_toTime(const fort::myrmidon::AntStaticValue * v) {
	return std::get<fort::myrmidon::Time>(*v);
}

fort::myrmidon::AntStaticValue fmAntStaticTime(const fort::myrmidon::Time & v) {
	return fort::myrmidon::AntStaticValue(v);
}



RCPP_MODULE(antStaticValue) {
	Rcpp::class_<fort::myrmidon::AntStaticValue>("fmAntStaticValue")
		.const_method("show",&fmAntStaticValue_show)
		.const_method("toBool",&fmAntStaticValue_toBool)
		.const_method("toInteger",&fmAntStaticValue_toInteger)
		.const_method("toNumeric",&fmAntStaticValue_toNumeric)
		.const_method("toString",&fmAntStaticValue_toString)
		.const_method("toTime",&fmAntStaticValue_toTime)
		;

	Rcpp::function("fmAntStaticBool",&fmAntStaticBool);
	Rcpp::function("fmAntStaticInteger",&fmAntStaticInteger);
	Rcpp::function("fmAntStaticNumeric",&fmAntStaticNumeric);
	Rcpp::function("fmAntStaticString",&fmAntStaticString);
	Rcpp::function("fmAntStaticTime",&fmAntStaticTime);

}
