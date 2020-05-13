#pragma once

#include <functional>

#include "Types.hpp"
#include "DenseMap.hpp"

#include <fort/myrmidon/Types.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class TagStatisticsHelper {
public:
	struct  Timed {
		TagStatistics::ByTagID TagStats;
		Time  Start,End;
	};

	static TagStatistics Create(TagID tagID,const Time & firstTime);

	static void UpdateGaps(TagStatistics & stat, const Time & lastSeen, const Time & currentTime);

	static TagStatistics::CountHeader ComputeGap(const Time & lastSeen, const Time & currentTime);

	typedef std::function<Timed ()> Loader;

	static Timed BuildStats(const std::string & hermesFile);

	template <typename InputIter>
	inline static Timed MergeTimed(const InputIter & begin, const InputIter & end) {
		if ( begin == end ) {
			return Timed();
		}
		std::sort(begin,end,
		          []( const Timed &  a, const Timed & b) {
			          return a.Start < b.Start;
		          });
		Timed res = *begin;
		for ( auto iter = begin + 1;
		      iter != end;
		      ++iter ) {
			Merge(res,*iter);
		}
		return res;
	}

	template <typename InputIter>
	inline static TagStatistics::ByTagID MergeSpaced(const InputIter & begin,
	                  const InputIter & end) {
		if ( begin == end ) {
			return TagStatistics::ByTagID();
		}
		auto res = *begin;
		for ( auto iter = begin + 1; iter != end; ++iter ) {
			Merge(res,*iter);
		}
		return res;
	}

	static void Merge(Timed & stats, const Timed & other);
	static void Merge(TagStatistics::ByTagID & stats, const TagStatistics::ByTagID & other);
	static TagStatistics MergeTimed(const TagStatistics & old, const Time & oldEnd,
	                                const TagStatistics & newer, const Time & newerStart);

	static TagStatistics MergeSpaced(const TagStatistics & a, const TagStatistics & b);

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
