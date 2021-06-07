#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;


void BindTime(py::module_ &);
void BindTypes(py::module_ &);
void BindAnt(py::module_ & );
void BindExperiment(py::module_ & );
void BindMatchers(py::module_ &);
void BindQuery(py::module_ &);
