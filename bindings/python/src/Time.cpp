#include <pybind11/pybind11.h>
#include <pybind11/operators.h>


#include <fort/time/Time.hpp>

namespace py = pybind11;



void BindDuration(py::module_ & m) {
	py::class_<fort::Duration>(m,"Duration")
		.def(py::init<int64_t>(),"Initialize the duration from an amount of nanoseconds")
		.def(py::init<>(),"The zero duration")
		.def("__str__",[](const fort::Duration & d) -> std::string {
			               std::ostringstream oss;
			               oss << d;
			               return oss.str();
		               })
		.def_readonly_static("Hour",&fort::Duration::Hour)
		.def_readonly_static("Minute",&fort::Duration::Minute)
		.def_readonly_static("Second",&fort::Duration::Second)
		.def_readonly_static("Millisecond",&fort::Duration::Millisecond)
		.def_readonly_static("Microsecond",&fort::Duration::Microsecond)
		.def_static("Parse",&fort::Duration::Parse,"Parses a string to a Duration")
		.def("Hours",&fort::Duration::Hours,"The duration amount in hours")
		.def("Minutes",&fort::Duration::Minutes,"The duration amount in minutes")
		.def("Seconds",&fort::Duration::Seconds,"The duration amount in seconds")
		.def("Milliseconds",&fort::Duration::Milliseconds,"The duration amount in milliseconds")
		.def("Microseconds",&fort::Duration::Microseconds,"The duration amount in microseconds")
		.def("Nanoseconds",&fort::Duration::Nanoseconds,"The duration amount in nanoseconds")
		.def(py::self + py::self)
		.def(py::self + int())
		.def("__radd__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return fort::Duration(v) + d;
		                },
			py::is_operator())
		.def(py::self - py::self)
		.def(py::self - int())
		.def("__rsub__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return fort::Duration(v) - d;
		                },
			py::is_operator())

		.def(py::self * py::self)
		.def(py::self * int())
		.def("__rmul__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return v * d;
		                },
			py::is_operator())
		.def(py::self < py::self)
		.def(py::self <= py::self)
		.def(py::self > py::self)
		.def(py::self >= py::self)
		.def(py::self == py::self)
		;

	py::implicitly_convertible<int64_t,fort::Duration>();


}


void BindTime(py::module_ & m) {
	BindDuration(m);


}
