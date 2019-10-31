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
