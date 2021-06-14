#include <pybind11/operators.h>
#include <pybind11/chrono.h>

#include "BindMethods.hpp"

#include <fort/time/Time.hpp>


void BindDuration(py::module_ & m) {
	py::class_<fort::Duration>(m,
	                           "Duration",
	                           R"pydoc(
Represents an amount of nanoseconds as signed 64-bit integer
)pydoc")
		.def(py::init<int64_t>(),"Initialize a Duration from an amount of nanoseconds")
		.def(py::init<>(),"Initialize a zero second Duration")
		.def("__str__",[](const fort::Duration & d) -> std::string {
			               std::ostringstream oss;
			               oss << d;
			               return oss.str();
		               })
		.def_readonly_static("Hour",
		                     &fort::Duration::Hour,
		                     "(py_fort_myrmidon.Duration): An hour as a Duration")
		.def_readonly_static("Minute",
		                     &fort::Duration::Minute,
		                     "(py_fort_myrmidon.Duration); A minute as a Duration")
		.def_readonly_static("Second",
		                     &fort::Duration::Second,
		                     "(py_fort_myrmidon.Duration); A second as a Duration")
		.def_readonly_static("Millisecond",
		                     &fort::Duration::Millisecond,
		                     "(py_fort_myrmidon.Duration); A millisecond as a Duration")
		.def_readonly_static("Microsecond",
		                     &fort::Duration::Microsecond,
		                     "(py_fort_myrmidon.Duration); A microsecond as a Duration")
		.def_static("Parse",
		            &fort::Duration::Parse,
		            py::arg("d"),
		            R"pydoc(
    Parses a string to a Duration.

    Args:
       d (str): a string in the format `[amount][unit]` as a
                duration. Valid units are
                'h','m','s','ms','us','ns'. The pattern can be
                repeated (i.e. '4m32s' is valid).

    Returns:
       py_fort_myrmidon.Duration: the parsed value

    Raises:
       Error: when the parsed amount will not hold in a 64-bit signed
       integer
)pydoc")
		.def("Hours",
		     &fort::Duration::Hours,
		     R"pydoc(
    this Duration in hours.

    Returns:
        float: the duration as an amount of hours
)pydoc")
		.def("Minutes",
		     &fort::Duration::Minutes,
		     R"pydoc(
    this Duration in minutes.

    Returns:
        float: the duration as an amount of minutes
)pydoc")
		.def("Seconds",
		     &fort::Duration::Seconds,
		     R"pydoc(
    this Duration in seconds.

    Returns:
        float: the duration as an amount of seconds
)pydoc")
		.def("Milliseconds",
		     &fort::Duration::Milliseconds,
		     R"pydoc(
    this Duration in milliseconds.

    Returns:
        float: the duration as an amount of milliseconds
)pydoc")
		.def("Microseconds",
		     &fort::Duration::Microseconds,
		     R"pydoc(
    this Duration in microseconds.

    Returns:
        float: the duration as an amount of microseconds
)pydoc")
		.def("Nanoseconds",
		     &fort::Duration::Nanoseconds,
		     R"pydoc(
    this Duration in nanoseconds.

    Returns:
        int: the duration as an amount of nanoseconds
)pydoc")
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


	py::class_<fort::Time>(m,
	                       "Time",
	                       R"pydoc(
    Represents a point in time.

    This object represents a point in Time, potentially +/-∞. It
    features operation to compare or measure a Duration between two
    Time.

    The operation manipulating Time object never modifies the original
    objects, and always return new allocated object, so Time object
    can be considered immuable.

    It also provides methods to convert to and from
    `datetime.datetime` object. In that case these objects are
    considered naïve: expressed in localtime, and ignoring any
    associated timezone information.


    It provides methods to convert to and from `time` and
    `datetime.timestamp()` float values. However for time around 2020,
    these only ensure a 10us precision, but Time objects are precise
    to the nanosecond.
)pydoc")
		.def(py::init<>(),"initialize a Time as the epoch")
		.def(py::init(&timeFromPythonTimestamp),
		     py::arg("timestamp"),
		     R"pydoc(

    Creates a Time from a float as returned by time.time() or
    datetime.timestamp()

    Note: timestamp are only guaranted to be precise to 10us for Time
    around year 2020.

    Args:
        timestamp (float): an amount of second since the epoch. Could
                           be `float('inf')` or `float('-inf')`.
`)pydoc")
		.def(py::init(&timeFromPythonDatetime),
		     py::arg("dt"),
		     R"pydoc(
    Constructor from a datetime.datetime object

    Creates a Time from a `datetime.datetime object`. The object will
    be treated as a local naive datetime, i.e. expressed in localtime
    and ignoring any time information.

    By default `fort-myrmidon` and `fort-studio` express Time in UTC.

    Args:
        dt (datetime.datetime): a naïve datetime. Please convert it to
                                localtime with `dt.astimezone(None)`
                                before passing it to this initializer
                                or some time offset could be
                                introduced.

)pydoc")
		.def_static("SinceEver",
		            &fort::Time::SinceEver,
		            R"pydoc(
    Returns:
        py_fort_myrmidon.Time: A Time representing -∞
)pydoc")
		.def_static("Forever",
		            &fort::Time::Forever,
		            		            R"pydoc(
    Returns:
        py_fort_myrmidon.Time: A Time representing +∞
)pydoc")
		.def_static("Now",
		            &fort::Time::Now,
		            R"pydoc(
    Returns:
        py_fort_myrmidon.Time: the current Time
)pydoc")
		.def_static("Parse",
		            &fort::Time::Parse,
		            py::arg("input"),
		            R"pydoc(
    Parses a RFC3339 string to a Time.

    Parses a RFC3339 string (i.e. '1970-01-01T00:00:00.000Z') to a
    Time

    Args:
        input (str): the string to parse

    Returns:
        py_fort_myrmidon.Time: a Time that represent input

    Raises:
        Error: if input is a Time that is not representable.

)pydoc")
		.def("ToTimestamp",
		     [](const fort::Time & t) -> double{
			     if ( t.IsInfinite() == true ) {
				     return t.IsForever() ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
			     }
			     auto ts = t.ToTimestamp();
			     double res = ts.seconds();
			     res += 1e-9 * ts.nanos();
			     return res;
		     },
		     R"pydoc(
    Returns:
        float: an amount of second since the epoch
)pydoc")
		.def("ToDateTime",
		     [](const fort::Time & t) -> std::chrono::system_clock::time_point {
			     if ( t.IsInfinite() == true ) {
				     throw std::runtime_error("Cannot cast " + t.Format() + " to a datetime.datetime object");
			     }
			     auto ts = t.ToTimestamp();
			     return std::chrono::system_clock::time_point(std::chrono::seconds(ts.seconds()) + std::chrono::nanoseconds(ts.nanos()));
		     },
		     R"pydoc(
    Returns:
        datetime.datetime: a naive datetime.datetime object.
)pydoc")
		.def("Add",
		     &fort::Time::Add,
		     py::arg("d"),
		     R"pydoc(
    Adds a Duration to a Time

    Note: `self` remains unchanged.

    Args:
        d (py_fort_myrmidon.Duration): the Duration to add

    Returns:
        py_fort_myrmidon.Time: a new Time representing `self + d`

    Raises:
        Error: if the resulting Time is not representable.
)pydoc")
		.def("Round",
		     &fort::Time::Round,
		     py::arg("d"),
		     R"pydoc(
    Rounds a Time to the closest Duration

    Rounds a Time to the closest Duration. Only multiple of seconds
and power of 10 of Nanosecond smaller than a second are supported.

    Args:
        d (py_fort_myrmidon.Duration): a multiple of a second or a power of 10 of a nanosecond.

    Returns:
        py_fort_myrmidon.Time: a new Time rounded to d
)pydoc")
		.def("Reminder",
		     &fort::Time::Reminder,
		     py::arg("d"),
		     R"pydoc(
    Gets the remaider Duration of self.Round(d)

    Args:
        d (py_fort_myrmidon.Duration): the duration to round to.

    Returns:
        py_fort_myrmidon.Duration: the reminder of `self.Round(d)`
)pydoc")
		.def("After",
		     &fort::Time::After,
		     py::arg("other"),
		     R"pydoc(
    Tests if this Time is after other

    Similar to `self > other`. `__gt__` operator is also provided.

    Args:
        other (py_fort_myrmidon.Time): the other Time to test.

    Returns:
        bool:  result of `self > other`
)pydoc")
		.def("Before",
		     &fort::Time::Before,
		     py::arg("other"),
		     R"pydoc(
    Tests if this Time is before other

    Similar to `self < other`. `__lt__` operator is also provided.

    Args:
        other (py_fort_myrmidon.Time): the other Time to test.

    Returns:
        bool:  result of `self < other`
)pydoc")
		.def("Equals",
		     &fort::Time::Equals,
		     py::arg("other"),
		     R"pydoc(
    Tests if this Time is exactly equal to other

    Similar to `self == other`. `__eq__` operator is also provided.

    Args:
        other (py_fort_myrmidon.Time): the other Time to test.

    Returns:
        bool:  result of `self == other`
)pydoc")
		.def("IsForever",
		     &fort::Time::IsForever,
		     R"pydoc(
    Tests if this Time is +∞

    Returns:
        bool : `true` if this time is Time.Forever()
)pydoc")
		.def("IsSinceEver",
		     &fort::Time::IsSinceEver,
		     R"pydoc(
    Tests if this Time is -∞

    Returns:
        bool : `true` if this time is Time.SinceEver()
)pydoc")
		.def("IsInfinite",
		     &fort::Time::IsInfinite,
		     R"pydoc(
    Tests if this Time is + or - ∞

    Returns:
        bool : `true` if this time is Time.SinceEver() or Time.Forever()
)pydoc")
		.def("Sub",
		     &fort::Time::Sub,
		     R"pydoc(
    Measure the Duration between two Time

    Similar to `self - other`. `__sub__` operator is also provided.

    Args:
        other (py_fort_myrmidon.Time): the other Time to substract.

    Returns:
        bool:  result of `self - other`

    Raises:
        Error: if the result would not fit in a Duration (i.e. if one
            of the Time.IsInfinite())

)pydoc")
		.def("__str__",&fort::Time::Format)
		.def("__repr__",&fort::Time::DebugString)
		.def(py::self == py::self)
		.def(py::self < py::self)
		.def(py::self <= py::self)
		.def(py::self > py::self)
		.def(py::self >= py::self)
		.def("__sub__",
		     []( const fort::Time & a, const fort::Time & b) -> fort::Duration {
			     return a.Sub(b);
		     },
		     py::is_operator())
		.def("__add__",
		     [](const fort::Time & t, const fort::Duration & d) -> fort::Time {
			     return t.Add(d);
		     },
		     py::is_operator())
		.def("__add__",
		     [](const fort::Time & t, const fort::Duration & d) -> fort::Time {
			     return t.Add(-1 * d);
		     },
		     py::is_operator())
		;

	py::implicitly_convertible<double,fort::Time>();
	py::implicitly_convertible<std::chrono::system_clock::time_point,fort::Time>();

}
