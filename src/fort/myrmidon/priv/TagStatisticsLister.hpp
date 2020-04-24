#pragma once

#include <functional>

#include "Types.hpp"
#include "DenseMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

struct TagStatistics {
	typedef std::shared_ptr<const TagStatistics> ConstPtr;
	typedef std::shared_ptr<TagStatistics>       Ptr;
	TagID    ID;
	Time     FirstSeen,LastSeen;

	TagStatistics(TagID tagID,const Time & firstTime);


	Eigen::Matrix<uint64_t,Eigen::Dynamic,1> Counts;

	enum CountHeader {
		TOTAL_SEEN        = 0,
		MULTIPLE_SEEN     = 1,
		GAP_500MS         = 2,
		GAP_1S            = 3,
		GAP_10S           = 4,
		GAP_1M            = 5,
		GAP_10M           = 6,
		GAP_1H            = 7,
		GAP_10H           = 8,
		GAP_MORE_THAN_10H = 9,
	};

	void UpdateGaps(const Time & lastSeen, const Time & currentTime);

	static CountHeader ComputeGap(const Time & lastSeen, const Time & currentTime);

};


class TagStatisticsLister {
public:
	typedef DenseMap<TagID,TagStatistics::Ptr> Stats;
	struct  TimedStats {
		Stats TagStats;
		Time  Start,End;
	};

	typedef std::function<TimedStats ()> Loader;

	static TimedStats BuildStats(const std::string & hermesFile);

	template <typename InputIter>
	inline static Stats Merge(const InputIter & begin, const InputIter & end) {
		if ( begin == end ) {
			return std::make_pair(Time(),Stats());
		}
		std::sort(begin,end,
		          []( const TimedStats &  a, const TimedStats & b) {
			          return a.Start < b.Start;
		          });
		auto & res = begin;
		for ( auto iter = begin + 1;
		      iter != end;
		      ++iter ) {
			Merge(res,iter);
		}
		return res->TagStats;
	}
private:
	static void Merge(TimedStats & stats, const TimedStats & other);
	static void  Merge(TagStatistics & old, const Time & oldEnd,
	                   const TagStatistics & newer, const Time & newerStart);

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
