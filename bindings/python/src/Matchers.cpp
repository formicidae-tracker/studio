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
	py::class_<Matcher,std::shared_ptr<Matcher>>(m,
	                                             "Matcher",
	                                             R"pydoc(
    A Matcher helps to build complex Query by adding one or several constraints.

    Matcher can be combined together with the `Matcher.Or()` and
    `Matcher.And()` Matcher.

  )pydoc")
		.def_static("AntID",
		            &Matcher::AntID,
		            py::arg("antID"),
		            R"pydoc(
    A Matcher that match an AntID.

    For `Query.ComputeAntTrajectories`, matches only Ant who match the given `antID`.

    For `Query.ComputeAntInteractions`, matches only interactions whos one of the ant matches `antID`.

    Args:
        antID (int): the AntID to match against.
    Returns:
        py_fort_myrmidon.Matcher: a matcher that matches antID.
)pydoc")
		.def_static("AntColumn",&Matcher::AntColumn,
		            py::arg("name"),py::arg("value"),
		            R"pydoc(
    A Matcher that matches a given user meta-data.

    Args:
        name (str): the name of the user meta-data column.
        value (str): the AntStaticValue to match against.
    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches Ant who
            current `name` value matches `value`.
)pydoc")
		.def_static("AntDistanceSmallerThan",
		            &Matcher::AntDistanceSmallerThan,
		            py::arg("distance"),
		            R"pydoc(
    A Matcher that matches ant distance.


)pydoc")
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
