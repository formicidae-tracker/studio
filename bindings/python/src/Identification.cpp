#include <pybind11/eigen.h>

#include <fort/myrmidon/Identification.hpp>

namespace py = pybind11;

void BindIdentification(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Identification,Identification::Ptr>(m,
	                                               "Identification"
	                                               R"pydoc(
    Identification relates TagID with Ant with time validity and
    geometric data.
)pydoc")
		.def_property_readonly("TagValue",
		                       &Identification::TagValue,
		                       " (int): the associated TagID of this identification")
		.def_property_readonly("TargetAntID",
		                       &Identification::TargetAntID,
		                       " (int): the associated AntID of this identification")
		.def_property_readonly("AntPosition",
		                       &Identification::AntPosition,
		                       " (numpy.ndarry(numpy.float64(2,1))): Ant center position relative to the tag center.")
		.def_property_readonly("AntAngle",
		                       &Identification::AntAngle,
		                       " (float): orientation difference between the ant orientation and its physical tag, expressed in radians.")
		.def_property("Start",
		              &Identification::Start,
		              &Identification::SetStart,
		              " (py_fort_myrmidon.Time): the first valid Time fort this identification, it can be Time.SinceEver()")
		.def_property("End",
		              &Identification::End,
		              &Identification::SetEnd,
		              " (py_fort_myrmidon.Time): the first invalid Time fort this identification, it can be Time.Forever()")
		.def_property("End",
		              &Identification::End,
		              &Identification::SetEnd,
		              " (py_fort_myrmidon.Time): the first invalid Time fort this identification, it can be Time.Forever()")
		.def("HasUserDefinedAntPose",
		     &Identification::HasUserDefinedAntPose,
		     R"pydoc(
    Returns:
        bool: `true` if the ant position relatively by its tag has
            been set by the user
)pydoc")
		.def("SetUserDefinedAntPose",
		     &Identification::SetUserDefinedAntPose,
		     py::arg("antPosition"),
		     py::arg("antAngle"),
		     R"pydoc(
    Sets an user defined ant position relatively to its tag, overiding
    the one computed from manual measurements.

    To revert to the automatically computed ones, use
    `py_fort_myrmidon.Identification.ClearUserDefinedAntPose(self)`

    Args:
        antPosition (numpy.ndarray(numpy.float64(2,1))): the position
            of the ant relatively to the tag center, in the tag
            reference frame
        antAngle (float): the ant angle relatively to the tag angle,
            in radians
 )pydoc")
		.def("ClearUserDefinedAntPose",
		     &Identification::ClearUserDefinedAntPose,
		     R"pydoc(
    Removes any user-defined ant-tag relative geometry data and
    re-enable the one computed from manual measurement in
    `fort-studio`.
)pydoc")
		.def("__str__",[](const Identification::Ptr & i) -> std::string {
			               std::ostringstream oss;
			               oss << *i;
			               return oss.str();
		               });
		;
}
