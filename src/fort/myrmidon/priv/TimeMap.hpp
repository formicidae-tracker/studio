#pragma once


#include <fort/myrmidon/Time.hpp>

#include <map>
#include <unordered_map>
#include <limits>

namespace fort {
namespace myrmidon {
namespace priv {

template <typename T,typename U>
class TimeMap {
public:

	inline void Insert(const T & key, const U & value , const Time::ConstPtr & time) {
		auto fi = d_map.find(key);
		if ( fi == d_map.end() ) {
			auto res = d_map.insert(std::make_pair(key,ValuesByTimestamp()));
			fi = res.first;
		}
		fi->second.insert(std::make_pair(Time::SortKey(time),value));
	}

	inline const U & At(const T & key, const Time & t) const {
		auto fi = d_map.find(key);
		if ( fi == d_map.end() || fi->second.empty() ) {
			throw std::out_of_range("Invalid key");
		}
		auto ti = fi->second.upper_bound(t.SortKey());
		if ( ti == fi->second.begin() ) {
			throw std::out_of_range("Invalid time");
		}
		return std::prev(ti)->second;
	}

	inline void Clear() {
		d_map.clear();
	}

private:
	typedef std::map<Time::SortableKey,U> ValuesByTimestamp;

	std::unordered_map<T,ValuesByTimestamp> d_map;

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
