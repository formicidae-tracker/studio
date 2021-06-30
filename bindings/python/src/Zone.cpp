#include <pybind11/stl.h>

namespace py = pybind11;

#include <fort/myrmidon/Zone.hpp>


void BindZoneDefinition(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ZoneDefinition>(m,
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
	py::class_<Zone>(m,
	                 "Zone",
	                 "Defines a named region of interest for tracking and interactions")
		.def_property("Name",
		     &Zone::Name,
		     &Zone::SetName,
		     " (str): the name of the Zone")
		.def_property_readonly("ID",
		                       &Zone::ID,
		                       " (int): the unique ID for this Zone")
		.def_property_readonly("Definitions",
		                       &Zone::Definitions,
		                       py::return_value_policy::reference_internal,
		                       " (List[py_fort_myrmidon.ZoneDefinition]): the definitions for this Zone")
		.def("AddDefinition",
		     [](Zone & zone, const Shape::List & shapes, const fort::Time & start, const fort::Time & end) -> ZoneDefinition & {
			     return * zone.AddDefinition(shapes,start,end);
		     },
		     py::arg("shapes"),
		     py::arg("start") = fort::Time::SinceEver(),
		     py::arg("end") = fort::Time::Forever(),
		     py::return_value_policy::reference_internal,
		     R"pydoc(
    Adds a new ZoneDefinition to this Zone

    Args:
        shapes (List[py_fort_myrmidon.Shape]): the geometry of the
            ZoneDefinition
        start (py_fort_myrmidon.Time): the first valid Time for the
            ZoneDefinition
        end (py_fort_myrmidon.Time): the first valid Time for the
            ZoneDefinition
    Returns:
        py_fort_myrmidon.ZoneDefinition: the new ZoneDefinition for
            this Zone
    Raises:
        ValueError: if start or end would make an overlapping
            definition with another Zone's ZoneDefinition
)pydoc")
		.def("DeleteDefinition",
		     &Zone::DeleteDefinition,
		     py::arg("index"),
		     R"pydoc(
    Removes a ZoneDefinition

    Args:
        index (int): the index in Zone.Definitions to remove.
    Raises:
        IndexError: if index >= len(Zone.Definitions)
)pydoc")
		;
}
