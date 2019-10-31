#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
namespace fort {

namespace myrmidon {

// A Duration represent time ellapsed between two Time
//
class Duration {
public:
	// constructor by an ammount of nanosecond
	Duration(int64_t ns);

	template <typename T,typename U>
	Duration( const std::chrono::duration<T,U> & duration)
		: d_nanoseconds(std::chrono::duration<int64_t,std::nano>(duration).count()) {}

	double Hours() const;
	double Minutes() const;
	double Seconds() const;
	double Milliseconds() const;
	double Microseconds() const;

	int64_t Nanoseconds() const {
		return d_nanoseconds;
	}

	static Duration Parse(const std::string & string);

	bool operator==(const Duration & other) const {
		return d_nanoseconds == other.d_nanoseconds;
	}

private:
	int64_t d_nanoseconds;
};

const static Duration Hour(3600e9);
const static Duration Minute(60e9);
const static Duration Second(1e9);
const static Duration Millisecond(1e6);
const static Duration Microsecond(1e3);
const static Duration Nanosecond(1);

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
	typedef uint64_t MonoclockID;
	static Time Now();
	static Time Date(int year, int month, int day, int hour, int minute, int seconds, int nanoseconds);
	static Time FromTimeT();
	static Time FromTimestamp();

	Time();
	Time(,uint64_t monoNanoseconds,MonoclockID ID);
	Time(int64_t seconds,int32_t nanoseconds,uint64_t monoNanoseconds,MonoclockID ID);

	Time Add(const Duration & d);
	bool After(const Time & t) const;
	bool Before(const Time & t) const;
	Duration Sub(const Time & time) const;


private:
	uint64_t    d_wall;
	uint64_t    d_monotonic;
	MonoclockID d_monotonicID;
};


} // namespace myrmidon

} // namespace fort


fort::myrmidon::Duration operator+(const fort::myrmidon::Duration & a,
                                   const fort::myrmidon::Duration & b) {
	return a.Nanoseconds() + b.Nanoseconds();
}

fort::myrmidon::Duration operator*(const fort::myrmidon::Duration & a,
                                   const fort::myrmidon::Duration & b) {
	return a.Nanoseconds() * b.Nanoseconds();
}

fort::myrmidon::Duration operator-(const fort::myrmidon::Duration & a,
                                   const fort::myrmidon::Duration & b) {
	return a.Nanoseconds() - b.Nanoseconds();
}

fort::myrmidon::Duration operator-(const fort::myrmidon::Duration & a) {
	return -a.Nanoseconds();
}



std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Duration & d) {
	using namespace fort::myrmidon;
	int64_t ns = d.Nanoseconds();
	if ( ns == 0 ) {
		return out << "0s";
	}
	std::string sign = "";

	if (ns == std::numeric_limits<int64_t>::min()) {
		return out << "-2562047h47m16.854775808s";
	}

	if (ns < 0) {
		sign = "-";
		ns = -ns;
	}

	if ( ns < Microsecond.Nanoseconds() ) {
		return out << sign << ns << "ns";
	}

	if ( ns < Millisecond.Nanoseconds() ) {
		return out << d.Microseconds() << "µs";
	}

	if ( ns < Second.Nanoseconds() ) {
		return out << d.Milliseconds() << "ms";
	}

	int64_t minutes = ns / 60e9;
	double seconds = Duration(ns % 60000000000LL).Seconds();



	if ( minutes == 0 ) {
		return out << d.Seconds() << "s";
	}

	int64_t hours = minutes / 60;
	minutes = minutes % 60;
	auto flags = out.flags();
	out << std::setprecision(12);
	if ( hours == 0 ) {
		out << sign << minutes << "m" << seconds << "s";
		out.setf(flags);
		return out;
	}

	out << sign << hours << "h" << minutes << "m" << seconds << "s";
	out.setf(flags);
	return out;
}
