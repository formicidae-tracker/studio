#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/chrono.h>


#include <fort/time/Time.hpp>

namespace py = pybind11;



void BindDuration(py::module_ & m) {
	py::class_<fort::Duration>(m,"Duration")
		.def(py::init<int64_t>(),"Initialize the duration from an amount of nanoseconds")
		.def(py::init<>(),"The zero Duration")
		.def("__str__",[](const fort::Duration & d) -> std::string {
			               std::ostringstream oss;
			               oss << d;
			               return oss.str();
		               })
		.def_readonly_static("Hour",&fort::Duration::Hour,"A constant representing an hour as a Duration")
		.def_readonly_static("Minute",&fort::Duration::Minute,"A constant representing a minute as a Duration")
		.def_readonly_static("Second",&fort::Duration::Second,"A constant representing a second as a Duration")
		.def_readonly_static("Millisecond",&fort::Duration::Millisecond,"A constant representing a millisecond as a Duration")
		.def_readonly_static("Microsecond",&fort::Duration::Microsecond,"A constant representing a microsecond as a Duration")
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

fort::Time timeFromPythonTimestamp(const double & t) {
	if ( std::isinf(t) ) {
		return t > 0 ? fort::Time::Forever() : fort::Time::SinceEver();
	}
	int64_t s = std::floor(t);
	int32_t ns = 1e9 * (t - s);
	return fort::Time::FromUnix(s,ns);
}

fort::Time timeFromPythonDatetime(const std::chrono::system_clock::time_point & t) {
	using namespace std::chrono;

	auto ns = duration_cast<nanoseconds>(t.time_since_epoch() % seconds(1));
	if ( ns.count() < 0 ) {
		ns += seconds(1);
	}

	std::time_t ltime = system_clock::to_time_t(time_point_cast<system_clock::duration>(t - ns));

	return fort::Time::FromUnix(ltime,ns.count());
}


void BindTime(py::module_ & m) {
	BindDuration(m);


	py::class_<fort::Time>(m,"Time")
		.def(py::init<>(),"Default constructor that returns the epoch as a Time")
		.def(py::init(&timeFromPythonTimestamp),R"pydoc(Creates a Time from a float as returned by time.time() or datetime.timestamp()

Creates a Time from an amount of second ellapsed since epoch. Since this amount is represented as a float, only a precision of 10us is guaranted for time around years 2020. Indeed, the iEEEE 754 standard for `double` only ensure a precision of 15 significant digit, which result only to 5 sub-second digits.

This function support the mapping of `float('inf')`/`float('-inf')` to `time.ForEver()/time.SinceEver()`)pydoc")
		.def(py::init(&timeFromPythonDatetime),R"pydoc(Constructor from a datetime.datetime object

Creates a Time from a datetime.datetime object. The object will be treated as a local naive datetime, i.e. not UTC, use datetime.datetime.astimezone(None) to convert the object to the appropriate timezone.
)pydoc")
		.def_static("SinceEver",&fort::Time::SinceEver,"A Time representing -∞")
		.def_static("Forever",&fort::Time::Forever,"A Time representing +∞")
		.def_static("Now",&fort::Time::Now,"A Time representing current time")
		.def_static("Parse",&fort::Time::Parse,"Parses a RFC3339 string (i.e. '1970-01-01T00:00:00.000Z' to a Time")
		.def("ToTimestamp",
		     [](const fort::Time & t) -> double{
			     if ( t.IsInfinite() == true ) {
				     return t.IsForever() ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
			     }
			     auto ts = t.ToTimestamp();
			     double res = ts.seconds();
			     res += 1e-9 * ts.nanos();
			     return res;
		     },"Returns a float as would time.time() or datetime.timestamp() would return")
		.def("ToDateTime",
		     [](const fort::Time & t) -> std::chrono::system_clock::time_point {
			     if ( t.IsInfinite() == true ) {
				     throw std::runtime_error("Cannot cast " + t.Format() + " to a datetime.datetime object");
			     }
			     auto ts = t.ToTimestamp();
			     return std::chrono::system_clock::time_point(std::chrono::seconds(ts.seconds()) + std::chrono::nanoseconds(ts.nanos()));
		     },R"pydoc(Returns a naive datetime.datetime object.

Converts the Time to naïve datetime.datetime object. In order to convert it to a timezone aware object, one can use datetime.astimezone().)pydoc")

		.def("Add",&fort::Time::Add,"Add a Duration to a Time")
		.def("Round",&fort::Time::Round,"Rounds a Time to the closest Duration")
		.def("Reminder",&fort::Time::Reminder,"Gets the remaining Duration of rounding by given Duration")
		.def("After",&fort::Time::After,"Tests if this Time is after another")
		.def("Before",&fort::Time::Before,"Tests if this Time is before another")
		.def("Equals",&fort::Time::Equals,"Tests if two Time are the same")
		.def("IsForever",&fort::Time::IsForever,"Tests if this Time is +∞")
		.def("IsSinceEver",&fort::Time::IsSinceEver,"Tests if this Time is -∞")
		.def("IsInfinite",&fort::Time::IsInfinite,"Tests if this Time is +/-∞")
		.def("Sub",&fort::Time::Sub,"Measure a Duration between two Time")
		.def("__str__",&fort::Time::Format)
		.def("__repr__",&fort::Time::DebugString)
		.def(py::self == py::self)
		.def(py::self < py::self)
		.def(py::self <= py::self)
		.def(py::self > py::self)
		.def(py::self >= py::self)
		;

	py::implicitly_convertible<double,fort::Time>();
	py::implicitly_convertible<std::chrono::system_clock::time_point,fort::Time>();

}
