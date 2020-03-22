#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>

#include <google/protobuf/timestamp.pb.h>

namespace fort {

namespace myrmidon {

// A Duration represent time ellapsed between two Time
//
// A duration could be negative. Why not using std::chrono::duration ?
// The C++ comittee took more than 9 years before figuring out that
// people may want to convert "1m" to a duration. Since <Time> should
// be re-implemented with a strong [golang
// time](https://golang.org/pkg/time) inspiration, why not doing the
// same for the associated <Duration>.
//
// This class aims to replicate a go syntax. For example to represent
// one hour, 10 minute, one may write:
//
// ```c++
// Duration d = 1 * Duration::Hour + 10 * Duration::Minute;
// ```
//
//
class Duration {
public:
	// constructor by an ammount of nanosecond
	// @ns the number of nanosecond
	inline Duration(int64_t ns)
		: d_nanoseconds(ns) {}

	// constructor from std::chrono::duration
	// @T the type holding
	template <typename T,typename U>
	Duration( const std::chrono::duration<T,U> & duration)
		: d_nanoseconds(std::chrono::duration<int64_t,std::nano>(duration).count()) {}


	// Gets the duration in hours
	// @return the duration in hour
	double Hours() const;

	// Gets the duration in minutes
	// @return the duration in minute
	double Minutes() const;

	// Gets the number of seconds
	// @return the duration in seconds
	double Seconds() const;

	// Gets the number of milliseconds
	// @return the duration in milliseconds
	double Milliseconds() const;

	// Gets the number of microseconds
	// @return the duration in microseconds
	double Microseconds() const;

	// Gets the number of nanoseconds
	// @return the duration in nanoseconds
	int64_t Nanoseconds() const {
		return d_nanoseconds;
	}

	// Parses a string to a Duration
	// @string the string to Parse in the form  `"<amount><unit>"`
	// @return the <Duration> represented by the string.
	//
	// Parses a <std::string> to a <Duration>. string must be of the
	// form `<amount><unit>` where `<amount>` is a value that may
	// contain a decimal point, and <unit> could be any of `h`, `m`,
	// `s`, `ms`, `us`, `µs` and `ns`. This pattern may be
	// repeated. For example `4m32s` is a valid input.
	//
	// It may throw <std::exception> on any parsing error.
	static Duration Parse(const std::string & string);


	bool operator==(const Duration & other) const {
		return d_nanoseconds == other.d_nanoseconds;
	}

	// An Hour
	const static Duration Hour;
	// A Minute
	const static Duration Minute;
	// A Second
	const static Duration Second;
	// A Millisecond
	const static Duration Millisecond;
	// A Microsecond
	const static Duration Microsecond;
	// A Nanosecond
	const static Duration Nanosecond;

	// The addition operator
	inline Duration operator+(const Duration & other) {
		return d_nanoseconds + other.d_nanoseconds;
	}

	inline Duration operator*(const fort::myrmidon::Duration & other) {
		return d_nanoseconds * other.d_nanoseconds;
	}

	inline Duration operator-(const fort::myrmidon::Duration & other) {
		return d_nanoseconds - other.d_nanoseconds;
	}

	inline Duration operator-() {
		return -d_nanoseconds;
	}



private:
	int64_t d_nanoseconds;
};



// Time represent a point in Time
//
// Time represent a point in Time. Why re-implementing one of this
// object and not using a `struct timeval` or a `std::chrono`. We are
// dealing with long living experiment on heterogenous system. Under
// this circunstances, we would like also to measure precise time
// difference. For this purpose we could use the framegrabber
// monolotic clock, which is timestamping every frame we acquired.
//
// Well the issue is that we cannot solely rely on this clock, as we
// may have several computers each with their own monolithic clock. Or
// even with a single computer, every time we started the tracking we
// must assume a new monotonic clock.
//
// We could use the wall clock but this clock may be resetted any
// time, and we would end up with issue where a time difference
// between two consecutive frame could be negative.
//
// Inspired from golang [time.Time](https://golang.org/pkg/time#Time)
// we propose an Object that store both a Wall time, and a Monotonic
// timestamp. But here we could have different monotonic timestamp. We
// try, whenever its possible (both <Time> have a monotonic time, and
// they are issued from the same monotonic clock), use that value for
// Time difference and Comparison. Otherwise the Wall clock value will
// be used with the issue regarding the jitter or Wall clock reset.
//
// Differentiaing Monotonic clock is done through <MonoclockID>
// values. The 0 value is reserved for the <SYSTEM_MONOTONIC_CLOCK>
// and which is used by <Now>. When reading saved monotonic Timestamp
// from the filesystem (as it is the case when reading data from
// different <TrackingDataDirectory> ), care must be taken to assign
// different <MonoclockID> for each of those reading. This class does
// not enforce any mechanism. The only entry point to define the
// <MonoclockID> is through the utility function
// <FromTimestampAndMonotonic>.
//
// Every time are considered UTC.
class Time {
public:
	typedef std::shared_ptr<Time> Ptr;
	typedef std::shared_ptr<const Time> ConstPtr;

	// Time values can overflow when performing operation on them.
	class Overflow : public std::runtime_error {
	public:
		Overflow(const std::string & clocktype)
			: std::runtime_error(clocktype + " value will overflow") {}
		virtual ~Overflow() {}
	};

	// ID for a Monotonic Clock
	typedef uint32_t MonoclockID;

	// Gets the current Time
	// @return the current time
	//
	// Gets the current <Time>. This time will both have a wall and a
	// monotonic clock reading associated with the
	// <SYSTEM_MONOTONIC_CLOCK>. Therefore such idioms:
	//
	// ```
	// Time start = Time::Now();
	// SomeFunction();
	// Duration ellapsed = Time::Now().Sub(start);
	// ```
	//
	// Will always return a positive Duration, even if the wall clock
	// has been reset between the two calls to <Now>
	static Time Now();

	// Creates a Time from `time_t`
	// @t the time_t value
	//
	// Creates a <Time> from `time_t`. The <Time> will not have any
	// monotonic clock value.
	static Time FromTimeT(time_t t);

	// Creates a Time from `struct timeval`
	// @t the `struct timeval`
	//
	// Creates a <Time> from `struct timeval`. The <Time> will not
	// have any monotonic clock value.
	static Time FromTimeval(const timeval & t);

	// Creates a Time from a protobuf Timestamp
	// @timestamp the `google.protobuf.Timestamp` message
	//
	// Creates a <Time> from a protobuf Timestamp. The <Time> will not
	// have any monotonic clock value.
	static Time FromTimestamp(const google::protobuf::Timestamp & timestamp);

	// Creates a Time from a protobuf Timestamp and an external Monotonic clock
	// @timestamp the `google.protobuf.Timestamp` message
	// @nsecs the external monotonic value in nanoseconds
	// @monoID the external monoID
	//
	// Creates a <Time> from a protobuf Timestamp and an external
	// monotonic clock. The two values should correspond to the same
	// physical time. It is an helper function to create accurate
	// <Time> from data saved in `fort.hermes.FrameReadout` protobuf
	// messages that saves both a Wall time value and a framegrabber
	// timestamp for each frame. It is the caller responsability to
	// manage <monoID> value for not mixing timestamp issued from
	// different clocks. Nothing prevent you to use
	// <SYSTEM_MONOTONIC_CLOCK> for the <monoID> value but the
	// behavior manipulating resulting time is undefined.
	static Time FromTimestampAndMonotonic(const google::protobuf::Timestamp & timestamp,
	                                      uint64_t nsecs,
	                                      MonoclockID monoID);


	// Parses from RFC 3339 date string format.
	// @input the string to parse
	//
	// Parses from [RFC 3339](https://www.ietf.org/rfc/rfc3339.txt)
	// date string format, i.e. string of the form
	// `1972-01-01T10:00:20.021-05:00`. It is merely a wrapper from
	// google::protobuf::time_util functions.
	static Time Parse(const std::string & input);


	// Converts to a `time_t`
	// @return `time_t`representing the <Time>.
	//
	// Converts to a `time_t`. Please note that time_t have a maximal
	// resolution of a second.
	time_t ToTimeT() const;

	// Converts to a `struct timeval`
	// @return `struct timeval`representing the <Time>.
	//
	// Converts to a `struct timeval`. Please note that time_t have a maximal
	// resolution of a microsecond.
	timeval ToTimeval() const;

	// Converts to a protobuf Timestamp message
	// @return the protobuf Timestamp representing the <Time>.
	google::protobuf::Timestamp ToTimestamp() const;

	// In-place conversion to a protobuf Timestamp
	// @timestamp the timestamp to modify to represent the <Time>
	void ToTimestamp(google::protobuf::Timestamp * timestamp) const;

	// Zero time constructor
	Time();

	// Adds a Duration to a Time
	// @d the <Duration> to add
	// @return a new <Time> distant by <d> from this <Time>
	Time Add(const Duration & d) const;

	// Reports if this time is after t
	// @t the <Time> to test against
	// @return `true` if this <Time> is strictly after <t>
	bool After(const Time & t) const;

	// Reports if this time is before t
	// @t the <Time> to test against
	// @return `true` if this <Time> is strictly before <t>
	bool Before(const Time & t) const;

	// Reports if this time is the same than t
	// @t the <Time> to test against
	// @return `true` if this <Time> is the same than <t>
	bool Equals(const Time & t) const;

	// Computes time difference with another time
	// @t the <Time> to substract to this one.
	// @return a <Duration> representing the time ellapsed between
	// <this> and <t>. It could be negative.
	Duration Sub(const Time & t) const;

	int64_t WallSeconds() const;
	int32_t WallNanos() const;

	// The <MonoclockID> reserved for the current system
	// `CLOCK_MONOTONIC`.
	const static MonoclockID SYSTEM_MONOTONIC_CLOCK = 0;

	// Number of nanoseconds in a second.
	const static uint64_t NANOS_PER_SECOND = 1000000000ULL;
	// Number of nanoseconds in a millisecond.
	const static uint64_t NANOS_PER_MILLISECOND = 1000000ULL;
	// Number of nanoseconds in a microsecond.
	const static uint64_t NANOS_PER_MICROSECOND = 1000ULL;

	// Reports the presence of a monotonic time value
	// @true if <this> contains a monotonic clock value.
	//
	// Reports the presence of a monotonic time value. Only <Time>
	// issued by <Now> or <FromTimestampAndMonotonic> contains a
	// monotonic time value.
	bool HasMono() const;

	// Returns the referred MonoclockID.
	// @return the <MonoclockID> designating the monotonic clock the
	//         monotonic time value refers to.
	//
	// Returns the referred <MonoclockID>. It throws std::exception if
	// this <Time> has no monotonic clock value (see <HasMono>).
	MonoclockID MonoID() const;


	// Returns the monotonic value.
	// @return the monotonic clock value.
	//
	// Returns the monotonic value. It throws std::exception if
	// this <Time> has no monotonic clock value (see <HasMono>).
	uint64_t MonotonicValue() const;

	std::string DebugString() const;


	// Helpers to convert (sec,nsec) to nsec
	// @sec the amount of second
	// @nsec the amount of nanos
	// @return sec * 1e9 + nsec if no overflow
	//
	// Helpers to convert (sec,nsec) to nsec. Throws <Overflow> on
	// overflow.
	static uint64_t MonoFromSecNSec(uint64_t sec, uint64_t nsec);

	class Comparator {
	public:
		bool operator()(const Time & a, const Time & b) const {
			return a.Before(b);
		}
	};

private:
	Time(int64_t wallsec, int32_t wallnsec, uint64_t mono, MonoclockID ID);


	const static uint32_t HAS_MONO_BIT = 0x80000000ULL;
	int64_t     d_wallSec;
	int32_t     d_wallNsec;
	uint64_t    d_mono;
	MonoclockID d_monoID;
};


} // namespace myrmidon

} // namespace fort

// C++ shenanigans
inline fort::myrmidon::Duration operator*(int64_t a,
                                          const fort::myrmidon::Duration & b) {
	return a * b.Nanoseconds();
}



// Formats a Duration
// @out the std::ostream to format to
// @d the <fort::myrmidon::Duration> to format
// @return a reference to <out>
//
// Formats the <fort::myrmidon::Duration> to the form
// "1h2m3.4s". Leading zero unit are omitted, and unit smaller than 1s
// uses a smalle unit ms us or ns. The zero duration formats to 0s. It
// mimic golang's
// [time.Duration.String()](https://golang.org/pkg/time/#Duration.String)
// behavior.
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Duration & d);

// Formats to RFC 3339 date string format
// @out the output iostream
// @t the <fort::myrmidon::Time> to format
// @return a reference to <out>
//
// Formats to [RFC 3339](https://www.ietf.org/rfc/rfc3339.txt) date
// string format, i.e. string of the form
// `1972-01-01T10:00:20.021Z`. It is merely a wrapper from
// google::protobuf::time_util functions.
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Time & t);

std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Time::ConstPtr & t );

bool operator== (const fort::myrmidon::Time & a,
                 const fort::myrmidon::Time & b);
