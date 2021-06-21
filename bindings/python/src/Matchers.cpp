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
		.def_static("AntMetaData",&Matcher::AntMetaData,
		            py::arg("key"),py::arg("value"),
		            R"pydoc(
    A Matcher that matches a given user meta data value.

    Args:
        key (str): the key to match from
        value (str): the AntStaticValue for key.
    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches Ant who
            current `key` meta data value matches `value`.
)pydoc")
		.def_static("AntDistanceSmallerThen",
		            &Matcher::AntDistanceSmallerThan,
		            py::arg("distance"),
		            R"pydoc(
    A Matcher that matches ant distance.

    For `Query.ComputeAntTrajectories`, it matches anything, as it
    requires two Ants.

    Args:
        distance (float): the distance in pixel to match

    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when the two
            Ant are within distance.
)pydoc")
		.def_static("AntDistanceGreaterThan",
		            &Matcher::AntDistanceGreaterThan,
		            py::arg("distance"),
		            R"pydoc(
    A Matcher that matches ant distance.

    For `Query.ComputeAntTrajectories`, it matches anything, as it
    requires two Ants.

    Args:
        distance (float): the distance in pixel to match

    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when the two
            Ant are further than distance.
)pydoc")
		.def_static("AntAngleSmallerThan",
		            &Matcher::AntAngleSmallerThan,
		            py::arg("angle"),
		            R"pydoc(
    A Matcher that matches ant angles.

    For `Query.ComputeAntTrajectories`, it matches anything, as it
    requires two Ants.

    Args:
        angle (float): the angle in radians

    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when the two
            Ant are facing the same direction
)pydoc")
		.def_static("AntAngleGreaterThan",
		            &Matcher::AntAngleGreaterThan,
		            py::arg("angle"),
		            R"pydoc(
    A Matcher that matches ant angles.

    For `Query.ComputeAntTrajectories`, it matches anything, as it
    requires two Ants.

    Args:
        angle (float): the angle in radians

    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when the two
            Ant are facing directions which are greater appart than angle
)pydoc")
		.def_static("And",
		            &MatcherAnd,
		            R"pydoc(
    Combines several Matcher together in conjuction

    Args:
        *args (py_fort_myrmidon.Matcher): several other Matcher
    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when all
            passed matcher also matches.
)pydoc")
		.def_static("Or",
		            &MatcherOr,
		            R"pydoc(
    Combines several Matcher together in disjunction

    Args:
        *args (py_fort_myrmidon.Matcher): several other Matcher
    Returns:
        py_fort_myrmidon.Matcher: a Matcher that matches when any of
            the passed matcher matches.
  )pydoc")
		.def_static("InteractionType",
		            &Matcher::InteractionType,
		            py::arg("type1"),
		            py::arg("type2"),
		            R"pydoc(
    Matches InteractionType (type1,type2) and (type2,type1).

    In the case of `Query.ComputeAntTrajectories` it matches anything.

    Args:
        type1 (int): the first AntShapeTypeID to match
        type2 (int): the second AntShapeTypeID to match
    Returns:
        py_fort_myrmidon.Matcher that matches interactions
            (type1,type2) or (type2,type1).
)pydoc")
		.def_static("AntDisplacement",
		            &Matcher::AntDisplacement,
		            py::arg("under"),
		            py::arg("minimumGap") = fort::Duration(0),
		            R"pydoc(
    Discards large ants displacement.

    Discards any trajectories and interactions where an Ant shows a
    displacement from one detected position to another larger than
    under. If minimumGap is larger than 0s, this check is enforced
    only if there are more than minimumGap ellapsed between two
    tracked positions.

    Args:
        under (float): maximum allowed Ant displacement in pixels
        minimumGap (py_fort_myrmidon.Duration): minimum tracking time
            between positions to enable the check.
    Returns:
        py_fort_myrmidon.Matcher that matches small Ant displacement.
)pydoc")
		.def("__str__",
		     [](const fort::myrmidon::Matcher::Ptr & m) -> std::string {
			     std::ostringstream oss;
			     oss << *m;
			     return oss.str();
		     })
		;

};
