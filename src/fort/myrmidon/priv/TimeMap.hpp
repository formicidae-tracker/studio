#pragma once


#include <fort/time/Time.hpp>

#include <map>
#include <unordered_map>
#include <limits>

namespace fort {
namespace myrmidon {
namespace priv {

template <typename T,typename U>
class TimeMap {
public:

	inline void Insert(const T & key, const U & value , const Time & time) {
		if ( time.IsForever() == true ) {
			throw std::invalid_argument("time value cannot be +∞");
		}
		auto fi = d_map.find(key);
		if ( fi == d_map.end() ) {
			auto res = d_map.insert(std::make_pair(key,ValuesByTime()));
			fi = res.first;
		}
		fi->second.insert(std::make_pair(time,value));
	}

	inline const U & At(const T & key, const Time & t) const {
		auto fi = d_map.find(key);
		if ( fi == d_map.end() || fi->second.empty() ) {
			throw std::out_of_range("Invalid key");
		}
		auto ti = fi->second.upper_bound(t);
		if ( ti == fi->second.begin() ) {
			throw std::out_of_range("Invalid time");
		}
		return std::prev(ti)->second;
	}

	inline void Clear() {
		d_map.clear();
	}

private:
	typedef std::map<Time,U> ValuesByTime;

	std::unordered_map<T,ValuesByTime> d_map;

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
