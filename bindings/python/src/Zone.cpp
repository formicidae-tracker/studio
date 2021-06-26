#include <pybind11/stl.h>

namespace py = pybind11;

#include <fort/myrmidon/Zone.hpp>


void BindZoneDefinition(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ZoneDefinition,ZoneDefinition::Ptr>(m,
	                                               "ZoneDefinition",
	                                               "Defines the geometry of a Zone during a time interval")
		.def_property("Shapes",
		     &ZoneDefinition::Shapes,
		     &ZoneDefinition::SetShapes,
		     " (List[py_fort_myrmidon.Shape]): the list of Shape that defines the geometry")
		.def_property("Start",
		              &ZoneDefinition::Start,
		              &ZoneDefinition::SetStart,
		              " (py_fort_myrmidon.Time): the first valid Time for this ZoneDefinition")
		.def_property("End",
		              &ZoneDefinition::End,
		              &ZoneDefinition::SetEnd,
		              " (py_fort_myrmidon.Time): the first invalid Time for this ZoneDefinition")
		;


}

void BindZone(py::module_ & m) {
	using namespace fort::myrmidon;
	BindZoneDefinition(m);
	py::class_<Zone,Zone::Ptr>(m,
	                           "Zone",
	                           "Defines a named region of interest for tracking and interactions")
		.def_property("Name",
		     &Zone::Name,
		     &Zone::SetName,
		     " (str): the name of the Zone")
		;
}
