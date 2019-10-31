#include "Time.hpp"

#include <time.h>

#include <stdexcept>
#include <map>

#include "utils/PosixCall.h"

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


uint64_t Time::MonoFromSecNSec(uint64_t sec, uint64_t nsec) {

	if ( sec > std::numeric_limits<int64_t>::max() / NANOS_PER_SECOND ) {
		throw Overflow("Mono");
	}

	uint64_t res = sec * NANOS_PER_SECOND;
	if ( res > std::numeric_limits<int64_t>::max() - nsec ) {
		throw Overflow("Mono");
	}

	return HAS_MONO_BIT | (res + nsec);
}


Time Time::Now() {
	struct timespec wall,mono;
	p_call(clock_gettime,CLOCK_REALTIME,&wall);
	p_call(clock_gettime,CLOCK_MONOTONIC,&mono);

	return Time (wall.tv_sec, wall.tv_nsec,
	             MonoFromSecNSec(mono.tv_sec, mono.tv_nsec),
	             MONOTONIC_CLOCK);
}

Time Time::FromTimeT(time_t value) {
	return Time(value,0 ,0,0);
}

Time Time::FromTimeval(const timeval & t) {
	return Time(t.tv_sec, t.tv_usec * NANOS_PER_MICROSECOND,0,0);
}

Time Time::FromTimestamp(const google::protobuf::Timestamp & timestamp) {
	return Time(timestamp.seconds(), timestamp.nanos(), 0, 0);
}

Time Time::FromTimestampAndMonotonic(const google::protobuf::Timestamp & timestamp,
                                     uint64_t nsecs,
                                     MonoclockID monoID) {
	return Time(timestamp.seconds(), timestamp.nanos(),
	            HAS_MONO_BIT | nsecs,
	            monoID);
}

Time::Time()
	: d_wallSec(0)
	, d_wallNsec(0)
	, d_mono(0)
	, d_monoID(0) {
}

Time::Time(int64_t wallSec, int32_t wallNsec, uint64_t mono, MonoclockID monoID)
	: d_wallSec(wallSec)
	, d_wallNsec(wallNsec)
	, d_mono(mono)
	, d_monoID(monoID) {
	while(d_wallNsec >= NANOS_PER_SECOND ) {
		if (d_wallSec == std::numeric_limits<int64_t>::max() ) {
			throw Overflow("Wall");
		}
		++d_wallSec;
		d_wallNsec -= NANOS_PER_SECOND;
	}

	while(d_wallNsec < 0) {
		if (d_wallSec == std::numeric_limits<int64_t>::min() ) {
			throw Overflow("Wall");
		}
		--d_wallNsec;
		d_wallNsec += NANOS_PER_SECOND;
	}
}

#define MONO_MASK ( HAS_MONO_BIT - 1 )

Time Time::Add(const Duration & d) const{
	uint64_t mono = 0;
	if ( d_mono & HAS_MONO_BIT ) {
		if ( int64_t(d_mono & MONO_MASK) > std::numeric_limits<int64_t>::max() + d.Nanoseconds() ) {
			throw Overflow("Mono");
		}
		mono = d_mono + d.Nanoseconds();
	}
	int64_t seconds = d.Nanoseconds() / NANOS_PER_SECOND;
	return Time(d_wallSec + seconds, d_wallNsec + (d.Nanoseconds() - seconds),mono,d_monoID);
}

bool Time::After(const Time & t) const {
	return Sub(t).Nanoseconds() > 0;
}

bool Time::Before(const Time & t) const {
	return Sub(t).Nanoseconds() < 0;
}

Duration Time::Sub(const Time & t) const {
	if ( (d_mono & HAS_MONO_BIT) == 0 ||
	     (t.d_mono & HAS_MONO_BIT) == 0 ||
	     d_monoID != t.d_monoID ) {

		int64_t seconds = d_wallSec - t.d_wallSec;
		int32_t nsecs = d_wallNsec - t.d_wallNsec;

		if ( seconds > std::numeric_limits<int64_t>::max() / NANOS_PER_SECOND ||
		     seconds < std::numeric_limits<int64_t>::min() / NANOS_PER_SECOND ) {
			throw Overflow("duration");
		}

		seconds *= NANOS_PER_SECOND;

		if ( (nsecs > 0 && seconds > std::numeric_limits<int64_t>::max() - nsecs) ||
		     (nsecs < 0 && seconds < std::numeric_limits<int64_t>::min() - nsecs) ) {
			throw Overflow("duration");
		}

		return seconds + nsecs;
	}
	return int64_t(d_mono & MONO_MASK) - int64_t(t.d_monoID & MONO_MASK);
}



std::ostream & operator<<(std::ostream & out,
                          const Duration & d) {

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
