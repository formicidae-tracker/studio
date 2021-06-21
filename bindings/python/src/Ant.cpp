#include <pybind11/pybind11.h>
#include <fort/myrmidon/Ant.hpp>

namespace py = pybind11;

void BindAnt(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Ant,Ant::Ptr> ant(m,
	                             "Ant",
	                             R"pydoc(
    Ant are the main object of interests of an Experiment , are
    identified with TagID from Identification, holds a virtual shape
    for collision detection, display information for visualization, or
    time variable user-defined key-value metadata.
)pydoc");

	py::enum_<Ant::DisplayState>(ant,
	                             "DisplayState",
	                             "possible display state for an Ant")
		.value("VISIBLE",Ant::DisplayState::VISIBLE,"the Ant is visible")
		.value("HIDDEN",Ant::DisplayState::HIDDEN,"the Ant is hidden")
		.value("SOLO",Ant::DisplayState::SOLO,"the Ant is visible and all other non-solo ant are hidden");


	ant.def("IdentifiedAt",
	        &Ant::IdentifiedAt,
	        py::arg("time"),
	        R"pydoc(
    Gets the TagID identifiying this Ant at a given time

    Args:
        time (py_fort_myrmidon.Time): the time we want an identification for
    Returns:
        int : the TagID that identifies this Ant at time.
    Raises:
        Error: an error when no tag identifies this Ant at time.
)pydoc")
		.def_property_readonly("Identifications",
		                       &Ant::Identifications,
		                       " (List[py_fort_myrmidon.Identification]): all Identification that target this Ant, ordered by validity time.")
		.def_property_readonly("AntID",
		                       &Ant::AntID,
		                       " (int): the AntID for this Ant")
		.def("FormattedID",
		     &Ant::FormattedID,
		     R"pydoc(
    Returns:
        str: the AntID formatted as a string as in `fort-studio`
)pydoc")
		.def_property("DisplayColor",
		              &Ant::DisplayColor,
		              &Ant::SetDisplayColor,
		              " (py_fort_myrmidon.DisplayColor): the color used to display the Ant in `fort-studio`")
		.def_property("DisplayState",
		              &Ant::DisplayStatus,
		              &Ant::SetDisplayStatus,
		              " (py_fort_myrmidon.Ant.DisplayState): the DisplayState in `fort-studio` for this Ant")
		.def("GetValue",
		     &Ant::GetValue,
		     py::arg("key"),
		     py::arg("time"),
		     R"pydoc(
    Gets user defined timed metadata.

    Args:
        key (str): the key to query
        time (py_fort_myrmidon.Time): the time, possibly infinite to query for
    Returns:
        py_fort_myrmidon.AntStaticValue: either a bool, int, float,
            str or Time that is the key value for Ant, or the
            Experiment default value for key if the value is not
            defined for this Ant.
    Raises:
        Error: if key is not a defined metadata key in Experiment
)pydoc")
		.def("SetValue",
		     &Ant::SetValue,
		     py::arg("key"),
		     py::arg("value"),
		     py::arg("time"),
		     R"pydoc(
    Sets a user defined timed metadata

    Args:
        key (str): the key to defined
        value (object): a bool, int, float str or Time to define key to.
        time (py_fort_myrmidon.Time): the first Time where key will be
            set to value. It can be Time.SinceEver()
     Raises:
        ValueError: if key is not defined in the Experiment
        ValueError: if time is Time.Forever()
        ValueError: if value is not the right type for key
)pydoc")
		.def("DeleteValue",
		     &Ant::DeleteValue,
		     py::arg("key"),
		     py::arg("time"),
		     R"pydoc(
    Clears a user defined timed metadata

    Args:
        key (str): the key to clear
        time (py_fort_myrmidon.Time): the time to clear key
     Raises:
        IndexError: if key was not previously set for time with
            SetValue.
)pydoc")
		.def("AddCapsule",
		     &Ant::AddCapsule,
		     py::arg("shapeTypeID"),
		     py::arg("capsule"),
		     R"pydoc(
    Adds a Capsule to the Ant virtual shape.

    Args:
        shapeTypeID (int): the AntShapeTypeID associated with the capsule
        capsule (py_fort_myrmidon.Capsule): the capsule to add
)pydoc")
		.def("Capsules",
		     &Ant::Capsules,
		     R"pydoc(
    Gets the capsules for this Ant.

    Returns:
        List[Tuple[int,py_fort_myrmidon.Capsule]]: a list of capsules
            and their type
)pydoc")
		.def("DeleteCapsule",
		     &Ant::DeleteCapsule,
		     py::arg("index"),
		     R"pydoc(
    Removes one of the shape

    Args:
        index (int): the index to remove in self.Capsules()
    Raises:
        IndexError: if index >= len(self.Capsules())
)pydoc")
		.def("ClearCapsules",
		     &Ant::ClearCapsules,
		     R"pydoc(
    Removes all capsule for this Ant
)pydoc")
		;

}
