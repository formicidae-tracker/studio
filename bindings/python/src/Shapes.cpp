#include <pybind11/eigen.h>

#include <fort/myrmidon/Shapes.hpp>

namespace py = pybind11;


void BindShapes(py::module_ & m) {
	py::class_<fort::myrmidon::Shape,fort::myrmidon::Shape::Ptr> shape(m,"Shape");

}
