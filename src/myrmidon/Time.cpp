#include "Time.hpp"

#include <stdexcept>
#include <map>

using namespace fort::myrmidon;

Duration::Duration(int64_t ns) : d_nanoseconds(ns) { }

using nanos = std::chrono::duration<uint64_t,std::nano>;

double Duration::Hours() const {
	using hourf = std::chrono::duration<double,std::ratio<3600>>;
	return hourf(nanos(d_nanoseconds)).count();
}

double Duration::Minutes() const {
	using minutef = std::chrono::duration<double,std::ratio<60>>;
	return minutef(nanos(d_nanoseconds)).count();
}

double Duration::Seconds() const {
	using secondef = std::chrono::duration<double,std::ratio<1>>;
	return secondef(nanos(d_nanoseconds)).count();
}

double Duration::Milliseconds() const {
	using millif = std::chrono::duration<double,std::milli>;
	return millif(nanos(d_nanoseconds)).count();
}

double Duration::Microseconds() const {
	using microf = std::chrono::duration<double,std::micro>;
	return microf(nanos(d_nanoseconds)).count();
}

Duration Duration::Parse(const std::string & i) {
	int64_t integer(0);
	double frac(0);
	bool neg(false);

#define throws(c) throw std::runtime_error("Could not parse '" + i + "':" + c)

	if ( i.empty() ) {
		throws("empty");
	}


	auto it = i.cbegin();

	if (*it == '-' || *it == '+' ) {
		neg = *it == '-';
		++it;
	}

	if ( i.cend() - it == 1 && *it == '0' ) {
		return 0;
	}


	bool ok = *it == '.';

	for (;  it != i.cend() && *it >= '0' && *it <= '9'; ++it) {
		ok = true;
		integer = integer * 10 + (*it - '0');
	}
	if ( integer < 0 ) {
		throws("integer overflow");
	}

	if ( ok == false ) {
		throws("need a number");
	}


	if (it != i.cend() && *it == '.' ) {
		++it;
		for (double base = 0.1;  it != i.cend() && *it >= '0' && *it <= '9'; ++it) {
			frac += (*it - '0') * base;
			base /= 10.0;
		}
	}
	if ( integer == 0 && frac == 0.0 ) {
		throws("empty number");
	}

	std::string unit;
	for ( ;it != i.cend() && ( *it < '0' || *it > '9') ; ++it ) {
		unit += *it;
	}

	static std::map<std::string,int64_t> units
		= {
		   {"ns",Nanosecond.Nanoseconds()},
		   {"us",Microsecond.Nanoseconds()},
		   {"µs",Microsecond.Nanoseconds()},
		   {"μs",Microsecond.Nanoseconds()},
		   {"ms",Millisecond.Nanoseconds()},
		   {"s",Second.Nanoseconds()},
		   {"m",Minute.Nanoseconds()},
		   {"h",Hour.Nanoseconds()},
	};
	auto fi = units.find(unit);
	if ( fi == units.end() ) {
		throws("unknown unit '" + unit + "'");
	}
	if ( integer > std::numeric_limits<int64_t>::max() / fi->second ) {
		throws("integer will overflow");
	}
	int64_t res = integer * fi->second;
	//fractionnal part cannot overflow
	int64_t resfrac = frac * fi->second;

	if ( res > std::numeric_limits<int64_t>::max() - resfrac ) {
		throws("will overflow");
	}
	res += resfrac;

	if (it == i.cend()) {
		if ( neg == true ) { return -res; };
		return res;
	}
	int64_t other = Parse(std::string(it,i.cend())).Nanoseconds();

	if ( neg == true ) {
		if ( -res < std::numeric_limits<int64_t>::min() + other ) {
			throws("overflow");
		}
		return - res - other;
	}
	if ( res > std::numeric_limits<int64_t>::max() - other ) {
		throws("overflow");
	}

#undef throws
	return  res + other;
}
