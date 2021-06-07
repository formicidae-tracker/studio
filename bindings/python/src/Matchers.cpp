#include <pybind11/pybind11.h>

#include <fort/myrmidon/Matchers.hpp>

namespace py = pybind11;

fort::myrmidon::Matcher::Ptr MatcherAnd(py::args args) {
	std::vector<fort::myrmidon::Matcher::Ptr> matchers;
	for( const auto & a : args ) {
		matchers.push_back(py::cast<fort::myrmidon::Matcher::Ptr>(a));
	}
	return fort::myrmidon::Matcher::And(matchers);
}

fort::myrmidon::Matcher::Ptr MatcherOr(py::args args) {
	std::vector<fort::myrmidon::Matcher::Ptr> matchers;
	for( const auto & a : args ) {
		matchers.push_back(py::cast<fort::myrmidon::Matcher::Ptr>(a));
	}
	return fort::myrmidon::Matcher::Or(matchers);
}


void BindMatchers(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Matcher,std::shared_ptr<Matcher>>(m,"Matcher")
		.def_static("AntID",&Matcher::AntID,
		            py::arg("antID"))
		.def_static("AntColumn",&Matcher::AntColumn,
		            py::arg("name"),py::arg("value"))
		.def_static("AntDistanceSmallerThan",&Matcher::AntDistanceSmallerThan,
		            py::arg("distance"))
		.def_static("AntDistanceGreaterThan",&Matcher::AntDistanceGreaterThan,
		            py::arg("distance"))
		.def_static("AntAngleSmallerThan",&Matcher::AntAngleSmallerThan,
		            py::arg("distance"))
		.def_static("AntAngleGreaterThan",&Matcher::AntAngleGreaterThan,
		            py::arg("distance"))
		.def_static("And",&MatcherAnd)
		.def_static("Or",&MatcherOr)
		;

};
