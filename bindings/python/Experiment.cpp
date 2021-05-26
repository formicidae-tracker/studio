#include <pybind11/pybind11.h>

#include <fort/myrmidon/myrmidon-version.h>
#include <fort/myrmidon/Experiment.hpp>

namespace py = pybind11;


PYBIND11_MODULE(py_fort_myrmidon, m) {
    m.doc() = "Bindings for libfort-myrmidon version " MYRMIDON_VERSION; // optional module docstring

    py::class_<fort::myrmidon::Experiment>(m, "Experiment")
	    .def(py::init(&fort::myrmidon::Experiment::Create), "Creates an experiment at the specified filesystem location")
	    .def_static("Open",&fort::myrmidon::Experiment::Open,"Opens an existing experiment")
	    .def("Save",&fort::myrmidon::Experiment::Save,"Save the experiment on the filesystem")
	    .def_property("Author",&fort::myrmidon::Experiment::Author,&fort::myrmidon::Experiment::SetAuthor, "The author of the experiment")
	    ;


}
