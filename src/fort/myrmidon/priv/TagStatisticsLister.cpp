#include "TagStatisticsLister.hpp"

#include <fort/hermes/FileContext.h>
#include <fort/hermes/Error.h>

#include "TimeUtils.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


TagStatistics::TagStatistics(TagID tagID,const Time & firstTime)
	: ID(tagID)
	, FirstSeen(firstTime)
	, LastSeen(firstTime)
	, Counts(10) {
	Counts.setZero();
	Counts(TOTAL_SEEN) = 1;
}

TagStatistics::CountHeader TagStatistics::ComputeGap(const Time & lastSeen, const Time & currentTime) {
	auto gap = currentTime.Sub(lastSeen);
	if ( gap < 0 ) {
		return CountHeader(0);
	}
	if ( gap < 500 * Duration::Millisecond ) {
		return GAP_500MS;
	}

	if ( gap < Duration::Second ) {
		return GAP_1S;
	}

	if ( gap < 10 * Duration::Second ) {
		return GAP_10S;
	}

	if ( gap < Duration::Minute ) {
		return GAP_1M;
	}

	if ( gap < 10 * Duration::Minute ) {
		return GAP_10M;
	}

	if ( gap < 1 * Duration::Hour ) {
		return GAP_1H;
	}

	if ( gap < 10 * Duration::Hour ) {
		return GAP_10H;
	}

	return GAP_MORE_THAN_10H;
}


TagStatisticsLister::TimedStats TagStatisticsLister::BuildStats(const std::string & hermesFile) {
	TimedStats res;

	auto & stats = res.TagStats;
	hermes::FileContext file(hermesFile,false);
	hermes::FrameReadout ro;

	struct LastSeen {
		FrameID FID;
		Time    FrameTime;
	};


	DenseMap<TagID,LastSeen> lastSeens;
	bool hasStart = false;
	for (;;) {
		try {
			file.Read(&ro);
		} catch ( const fort::hermes::EndOfFile & ) {
			for ( const auto & [tagID,last] : lastSeens ) {
				if ( last.FrameTime < res.End ) {
					stats.at(tagID)->UpdateGaps(last.FrameTime,res.End);
				}
			}
			return res;
		} catch ( const std::exception & e ) {
			throw std::runtime_error("Could not build statistic for '"
			                         + hermesFile + "':" + e.what());
		}
		FrameID current = ro.frameid();
		// current time stripped from any monotonic data
		auto currentTime = TimeFromFrameReadout(ro,1).Round(-1);
		if ( hasStart == false ) {
			hasStart = true;
			res.Start = currentTime;
		}

		res.End = currentTime;

		for ( const auto & tag : ro.tags() ) {
			auto key = tag.id() + 1;
			if ( stats.count(key) == 0 ) {
				lastSeens.insert(std::make_pair(key,LastSeen{current,currentTime}));
				auto tagStats = std::make_shared<TagStatistics>(tag.id(),currentTime);
				if ( currentTime > res.Start ) {
					tagStats->UpdateGaps(res.Start,currentTime);
				}
				stats.insert(std::make_pair(key,tagStats));
			} else {
				auto & last = lastSeens.at(key);
				auto & tagStats = stats.at(key);
				if ( last.FID == current ) {
					tagStats->Counts(TagStatistics::CountHeader::MULTIPLE_SEEN) += 1;
				} else {
					tagStats->Counts(TagStatistics::CountHeader::TOTAL_SEEN) += 1;
					if ( last.FID < current-1) {
						tagStats->UpdateGaps(last.FrameTime,currentTime);
					}
					tagStats->LastSeen = currentTime;
				}
				last.FID = current;
				last.FrameTime = currentTime;
			}
		}
	}
}
void TagStatistics::UpdateGaps(const Time & lastSeen, const Time & currentTime) {
	auto gap = ComputeGap(lastSeen,currentTime);
	if ( gap < GAP_500MS ) {
		return;
	}
	Counts(gap) += 1;
}

void TagStatisticsLister::Merge(TimedStats & stats, const TimedStats & other) {
	if ( stats.End > other.Start ) {
		throw std::runtime_error("Could ony merge time-upward");
	}
	for ( const auto & [tagID,tagStats] : other.TagStats ) {
		auto fi = stats.TagStats.find(tagID);
		if ( fi == stats.TagStats.end() ) {
			stats.TagStats.insert(std::make_pair(tagID,tagStats));
		} else {
			*(fi->second) = MergeTimed(*(fi->second),stats.End,*tagStats,other.Start);
		}
	}
	stats.End = other.End;
}

void TagStatisticsLister::Merge(Stats & stats, const Stats & other) {
	for ( const auto & [tagID,tagStats] : other ) {
		auto fi = stats.find(tagID);
		if ( fi == stats.end() ) {
			stats.insert(std::make_pair(tagID,tagStats));
		} else {
			*(fi->second) = MergeSpaced(*(fi->second),*tagStats);
		}
	}
}


TagStatistics
TagStatisticsLister::MergeTimed(const TagStatistics & old, const Time & oldEnd,
                                const TagStatistics & newer, const Time & newerStart) {
	if ( old.ID != newer.ID ) {
		throw std::invalid_argument("Mismatched ID "
		                            + std::to_string(newer.ID)
		                            + " (expected:" + std::to_string(old.ID) + ")");
	}


	if ( oldEnd > newerStart ) {
		throw std::runtime_error("Older statistics must happen after newer one");
	}
	TagStatistics res(old);
	res.Counts += newer.Counts;
	bool computeNew = false;
	if ( newer.FirstSeen > newerStart ) {
		res.Counts(TagStatistics::ComputeGap(newerStart,newer.FirstSeen)) -= 1;
		computeNew = true;
	}
	if ( res.LastSeen < oldEnd ) {
		res.Counts(TagStatistics::ComputeGap(old.LastSeen,oldEnd)) -= 1;
		computeNew = true;
	}
	if ( computeNew ==  true ) {
		res.Counts(TagStatistics::ComputeGap(old.LastSeen,newer.FirstSeen)) += 1;
	}

	res.LastSeen = newer.LastSeen;
	return res;
}

TagStatistics TagStatisticsLister::MergeSpaced(TagStatistics & a, TagStatistics & b) {
	if ( a.ID == b.ID ) {
		throw std::invalid_argument("Mismatched ID "
		                            + std::to_string(a.ID)
		                            + " (expected:" + std::to_string(b.ID) + ")");
	}
	TagStatistics res(a);
	res.Counts += b.Counts;
	res.LastSeen = a.LastSeen > b.LastSeen ? a.LastSeen : b.LastSeen;
	res.FirstSeen = a.FirstSeen < b.FirstSeen ? a.FirstSeen : b.FirstSeen;
	return res;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
