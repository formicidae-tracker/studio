#pragma once

#include <cstdint>
#include <chrono>
#include <string>

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
