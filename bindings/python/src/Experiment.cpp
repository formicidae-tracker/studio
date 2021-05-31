#include <pybind11/pybind11.h>

#include <fort/myrmidon/Experiment.hpp>

namespace py = pybind11;

void BindExperiment(py::module_ & m) {
	py::class_<fort::myrmidon::Experiment>(m, "Experiment")
		.def(py::init(&fort::myrmidon::Experiment::Create), "Creates an experiment at the specified filesystem location")
		.def_static("Open",&fort::myrmidon::Experiment::Open,"Opens an existing experiment")
		.def("Save",&fort::myrmidon::Experiment::Save,"Save the experiment on the filesystem")
		.def_property("Author",&fort::myrmidon::Experiment::Author,&fort::myrmidon::Experiment::SetAuthor, "The author of the experiment")
		;
}
