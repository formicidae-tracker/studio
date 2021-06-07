#include <pybind11/pybind11.h>

#include "BindMethods.hpp"

#ifndef VERSION_INFO
#include <fort/myrmidon/myrmidon-version.h>
#else
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#endif

namespace py = pybind11;




PYBIND11_MODULE(py_fort_myrmidon, m) {
    m.doc() = "Bindings for libfort-myrmidon"; // optional module docstring

    BindTypes(m);
    BindAnt(m);
    BindExperiment(m);

    BindMatchers(m);
    BindQuery(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
