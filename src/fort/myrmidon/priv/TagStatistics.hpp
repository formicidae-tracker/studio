#pragma once

#include <functional>

#include "Types.hpp"
#include "DenseMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class TagStatistics {
public:
	typedef std::shared_ptr<const TagStatistics> ConstPtr;
	typedef std::shared_ptr<TagStatistics>       Ptr;
	typedef DenseMap<TagID,TagStatistics::Ptr>   ByTagID;

	struct  Timed {
		ByTagID TagStats;
		Time  Start,End;
	};



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
		GAP_MORE          = 9,
	};

	void UpdateGaps(const Time & lastSeen, const Time & currentTime);

	static CountHeader ComputeGap(const Time & lastSeen, const Time & currentTime);


	typedef std::function<Timed ()> Loader;

	static Timed BuildStats(const std::string & hermesFile);

	template <typename InputIter>
	inline static ByTagID MergeTimed(const InputIter & begin, const InputIter & end) {
		if ( begin == end ) {
			return ByTagID();
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
		return res.TagStats;
	}

	template <typename InputIter>
	inline static ByTagID MergeSpaced(const InputIter & begin,
	                  const InputIter & end) {
		if ( begin == end ) {
			return ByTagID();
		}
		auto res = *begin;
		for ( auto iter = begin + 1; iter != end; ++iter ) {
			Merge(res,*iter);
		}
		return res;
	}

private:
	static void Merge(Timed & stats, const Timed & other);
	static void Merge(ByTagID & stats, const ByTagID & other);
	static TagStatistics MergeTimed(const TagStatistics & old, const Time & oldEnd,
	                                const TagStatistics & newer, const Time & newerStart);

	static TagStatistics MergeSpaced(TagStatistics & a, TagStatistics & b);

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
