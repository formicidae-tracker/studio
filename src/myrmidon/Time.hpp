#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>

#include <google/protobuf/util/time_util.h>

namespace fort {

namespace myrmidon {

// A Duration represent time ellapsed between two Time
//
// A duration could be negative. Why not using std::chrono::duration ?
// The C++ comittee took more than 9 years before figuring out that
// people may want to convert "1m" to a duration. Since <Time> should
// be re-implemented with a strong [golang](https://golang.org)
// inspiration, why not doing the same for the associated <Duration>.
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
// Inspired from golang [time.Time](https://golang.org/pkg/time) we
// propose an Object that store both a Wall time, and a Monotonic
// timestamp. But here we could have different monotonic timestamp. We
// try, whenever its possible (both Time have a monotonic time, and
// they are issued from the same monotonic clock), use that value for
// Time difference and Comparison. Otherwise the Wall clock value will
// be used with the issue regarding the jitter or Wall clock reset.
//
// Every time are considered UTC.
class Time {

	class Overflow : public std::runtime_error {
	public:
		Overflow(const std::string & clocktype)
			: std::runtime_error(clocktype + " value will overflow") {}
		virtual ~Overflow() {}
	};

	typedef uint32_t MonoclockID;

	static Time Now();

	static Time FromTimeT(time_t value);
	static Time FromTimeval(const timeval & );
	static Time FromTimestamp(const google::protobuf::Timestamp & timestamp);
	static Time FromTimestampAndMonotonic(const google::protobuf::Timestamp & timestamp,
	                                      uint64_t nsecs,
	                                      MonoclockID monotonic);

	Time();

	Time Add(const Duration & d) const;
	bool After(const Time & t) const;
	bool Before(const Time & t) const;
	Duration Sub(const Time & time) const;


	const static MonoclockID MONOTONIC_CLOCK = 0;

	const static uint64_t NANOS_PER_SECOND = 1000000000;
	const static uint64_t NANOS_PER_MILLISECOND = 1000000;
	const static uint64_t NANOS_PER_MICROSECOND = 1000;

private:
	Time(int64_t wallsec, int32_t wallnsec, uint64_t mono,MonoclockID ID);

	static uint64_t MonoFromSecNSec(uint64_t sec, uint64_t nsec);

	const static uint64_t HAS_MONO_BIT = 0x8000000000000000ULL;
	int64_t     d_wallSec;
	int32_t     d_wallNsec;
	uint64_t    d_mono;
	MonoclockID d_monoID;
};


} // namespace myrmidon

} // namespace fort



inline fort::myrmidon::Duration operator*(int64_t a,
                                          const fort::myrmidon::Duration & b) {
	return a * b.Nanoseconds();
}



std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Duration & d);
