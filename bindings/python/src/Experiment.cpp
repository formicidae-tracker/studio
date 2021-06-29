#include <pybind11/pybind11.h>

#include <fort/myrmidon/Experiment.hpp>

namespace py = pybind11;

void BindExperiment(py::module_ & m) {
	using namespace fort::myrmidon;

	py::class_<Experiment>(m,
	                                       "Experiment",
	                                       "An Experiment is the main entry point of `fort-myrmidon`")
		.def(py::init(&fort::myrmidon::Experiment::Create),
		     py::arg("filepath"),
		     R"pydoc(
    Initialize an experiment from a specified filesystem
    location. This location will be used to determine relative path to
    the tracking data.

    Args:
        filepath (str): the wanted filesystem path to the experiment.
    Returns:
        py_fort_myrmidon.Experiment: a new empty Experiment associated with filepath
)pydoc")
		.def_static("Open",
		            &Experiment::Open,
		            py::arg("filepath"),
		            R"pydoc(
    Opens an existing Experiment on the filesystem

    Args:
        filepath (str): the filepath to open.
    Returns:
        py_fort_myrmidon.Experiment: the Experiment located at filepath
    Raises:
        Error: if filepath does not contains a valid Experiment or
            associated TrackingData is not accessible.
)pydoc")
		.def_static("OpenDataLess",
		            &Experiment::OpenDataLess,
		            py::arg("filepath"),
		            R"pydoc(
    Opens an existing Experiment on the filesystem in dataless mode.

    In dataless mode, no associated tracking data will be opened, but
    a TrackingSolver can be used to identify Ants in live tracking
    situation.

    Args:
        filepath (str): the filepath to open.
    Returns:
        py_fort_myrmidon.Experiment: the Experiment located at filepath
    Raises:
        Error: if filepath does not contains a valid Experiment
)pydoc")

		.def("Save",
		     &Experiment::Save,
		     py::arg("filepath"),
		     R"pydoc(
    Saves the experiment on the filesystem.

    Args:
        filepath: the filepath to save the experiment to.
    Raises:
        ValueError: if filepath would change the directory of the
            Experiment on the filesystem.
)pydoc")
		.def_property_readonly("AbsoluteFilePath",
		                       &Experiment::AbsoluteFilePath,
		                       " (str): the absolute filepath of the Experiment")
		.def("CreateSpace",
		     &Experiment::CreateSpace,
		     py::arg("name"),
		     py::return_value_policy::reference_internal,
		     R"pydoc(
    Creates a new Space in this Experiment.

    Args:
        name (str): the name for the new space
    Returns:
        py_fort_myrmidon.Space: the newly created Space
)pydoc")
		.def("DeleteSpace",
		     &Experiment::DeleteSpace,
		     py::arg("spaceID"),
		     R"pydoc(
    Deletes a Space from this Experiment.

    Args:
        spaceID (str): the spaceID of this space
    Raises:
        IndexError: if spaceID is not a valid SpaceID for this Experiment.
)pydoc")
		.def_property_readonly("Spaces",
		                       &Experiment::Spaces,
		                       py::return_value_policy::reference_internal,
		                       " (Dict[int,py_fort_myrmidon.Space]): this Experiment space indexed by their SpaceID")

		.def("AddTrackingDataDirectory",
		     &Experiment::AddTrackingDataDirectory,
		     py::arg("spaceID"),
		     py::arg("filepath"),
		     R"pydoc(
    Adds a tracking data directory to the Experiment.

    Args:
        spaceID (int): the space to add the tracking data directory
            to.
        filepath (str): the filepath to the tracking data directory.
    Returns:
        str: the relative path from self.AbsoluteFilePath to filepath,
            that will be the URI to identify the tracking data
            directory.
    Raises:
        ValueError: if spaceID is not valid for this Experiment.
        Error: if filepath is not a valid tracking data directory.
)pydoc")
		.def("RemoveTrackingDataDirectory",
		     &Experiment::RemoveTrackingDataDirectory,
		     py::arg("URI"),
		     R"pydoc(
    Removes a tracking data directory from the Experiment.

    Args:
        URI (str): the URI that identifies the tracking data directory
    Raises:
        IndexError: if URI does not identifies a tracking data
            directory in this experiment.
)pydoc")
		.def_property("Author",
		              &Experiment::Author,
		              &Experiment::SetAuthor,
		              " (str): the author of the experiment")
		;
}
