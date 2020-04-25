#include "TagStatisticsLister.hpp"

#include <fort/hermes/FileContext.h>
#include <fort/hermes/Error.h>

#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>

#include <fort/myrmidon/TagStatisticsCache.pb.h>


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

	return GAP_MORE;
}

static int CURRENT_CACHE_VERSION = 1;

typedef proto::FileReadWriter<pb::TagStatisticsCacheHeader,
                              pb::TagStatistics> RW;

fs::path cacheFilePath(const std::string & hermesFile ) {
	return fs::path(hermesFile).replace_extension(".statcache");
}

TagStatistics::Ptr LoadStatistics(const pb::TagStatistics & pb) {
	auto start = Time::FromTimestamp(pb.firstseen());
	auto end = Time::FromTimestamp(pb.lastseen());

	auto res = std::make_shared<TagStatistics>(pb.id(),start);
	res->LastSeen = end;
	res->Counts << pb.totalseen(),
		pb.multipleseen(),
		pb.gap500ms(),
		pb.gap1s(),
		pb.gap10s(),
		pb.gap1m(),
		pb.gap10m(),
		pb.gap1h(),
		pb.gap10h(),
		pb.gapmore();
	return res;
}

void SaveStatistics(pb::TagStatistics * pb, const TagStatistics::ConstPtr & tagStats) {
	pb->set_id(tagStats->ID);
	tagStats->FirstSeen.ToTimestamp(pb->mutable_firstseen());
	tagStats->LastSeen.ToTimestamp(pb->mutable_lastseen());
	pb->set_totalseen(tagStats->Counts(TagStatistics::TOTAL_SEEN));
	pb->set_multipleseen(tagStats->Counts(TagStatistics::MULTIPLE_SEEN));
	pb->set_gap500ms(tagStats->Counts(TagStatistics::GAP_500MS));
	pb->set_gap1s(tagStats->Counts(TagStatistics::GAP_1S));
	pb->set_gap10s(tagStats->Counts(TagStatistics::GAP_10S));
	pb->set_gap1m(tagStats->Counts(TagStatistics::GAP_1M));
	pb->set_gap10m(tagStats->Counts(TagStatistics::GAP_10M));
	pb->set_gap1h(tagStats->Counts(TagStatistics::GAP_1H));
	pb->set_gap10h(tagStats->Counts(TagStatistics::GAP_10H));
	pb->set_gapmore(tagStats->Counts(TagStatistics::GAP_MORE));
}


TagStatisticsLister::TimedStats loadFromCache(const std::string & hermesFile) {

	TagStatisticsLister::TimedStats res;
	RW::Read(cacheFilePath(hermesFile),
	         [&res](const pb::TagStatisticsCacheHeader & pb) {
		         if ( pb.version() != CURRENT_CACHE_VERSION) {
			         throw std::runtime_error("Mismatched cache version "
			                                  + std::to_string(pb.version())
			                                  + " (expected:"
			                                  + std::to_string(CURRENT_CACHE_VERSION));
		         }
		         if ( pb.has_start() == false || pb.has_end() == false ){
			         throw std::runtime_error("Missing start or end time");
		         }

		         res.Start = Time::FromTimestamp(pb.start());
		         res.End = Time::FromTimestamp(pb.end());
	         },
	         [&res] ( const pb::TagStatistics & pb) {
		         res.TagStats.insert(std::make_pair(pb.id()+1,LoadStatistics(pb)));
	         });
	return res;
}

void saveToCache(const std::string & hermesFile, const TagStatisticsLister::TimedStats & stats) {
	pb::TagStatisticsCacheHeader h;
	h.set_version(CURRENT_CACHE_VERSION);
	stats.Start.ToTimestamp(h.mutable_start());
	stats.End.ToTimestamp(h.mutable_end());
	std::vector<RW::LineWriter> lines;
	for ( const auto & [tagID,tagStats] : stats.TagStats ) {
		lines.push_back([&tagStats](pb::TagStatistics & line) {
			                SaveStatistics(&line,tagStats);
		                });
	}
	RW::Write(cacheFilePath(hermesFile),
	          h,
	          lines);
}


TagStatisticsLister::TimedStats TagStatisticsLister::BuildStats(const std::string & hermesFile) {
	try {
		return loadFromCache(hermesFile);
	} catch ( const std::exception & ) {

	}

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

			try {
				saveToCache(hermesFile,res);
			} catch ( const std::exception & ) {

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
