#include "TagStatisticsCache.hpp"


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


const uint32_t TagStatisticsCache::CACHE_VERSION = 2;

const std::string TagStatisticsCache::CACHE_PATH = "tags_statistics.cache";

static TagStatistics LoadStatistics(const pb::TagStatistics & pb) {
	auto start = Time::FromTimestamp(pb.firstseen());
	auto end = Time::FromTimestamp(pb.lastseen());

	auto res = TagStatisticsHelper::Create(pb.id(),start);
	res.LastSeen = end;
	res.Counts << pb.totalseen(),
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

static void SaveStatistics(pb::TagStatistics * pb, const fort::myrmidon::TagStatistics & tagStats) {
	pb->set_id(tagStats.ID);
	tagStats.FirstSeen.ToTimestamp(pb->mutable_firstseen());
	tagStats.LastSeen.ToTimestamp(pb->mutable_lastseen());
	pb->set_totalseen(tagStats.Counts(TagStatistics::TOTAL_SEEN));
	pb->set_multipleseen(tagStats.Counts(TagStatistics::MULTIPLE_SEEN));
	pb->set_gap500ms(tagStats.Counts(TagStatistics::GAP_500MS));
	pb->set_gap1s(tagStats.Counts(TagStatistics::GAP_1S));
	pb->set_gap10s(tagStats.Counts(TagStatistics::GAP_10S));
	pb->set_gap1m(tagStats.Counts(TagStatistics::GAP_1M));
	pb->set_gap10m(tagStats.Counts(TagStatistics::GAP_10M));
	pb->set_gap1h(tagStats.Counts(TagStatistics::GAP_1H));
	pb->set_gap10h(tagStats.Counts(TagStatistics::GAP_10H));
	pb->set_gapmore(tagStats.Counts(TagStatistics::GAP_MORE));
}


TagStatisticsHelper::Timed
TagStatisticsCache::Load(const fs::path & tddAbsolutePath) {
	TagStatisticsHelper::Timed res;
	ReadWriter::Read(tddAbsolutePath / CACHE_PATH ,
	                 [&res](const pb::TagStatisticsCacheHeader & pb) {
		                 if ( pb.version() != CACHE_VERSION) {
			                 throw std::runtime_error("Mismatched cache version "
			                                          + std::to_string(pb.version())
			                                          + " (expected:"
			                                          + std::to_string(CACHE_VERSION));
		                 }
		                 if ( pb.has_start() == false || pb.has_end() == false ){
			                 throw std::runtime_error("Missing start or end time");
		                 }

		                 res.Start = Time::FromTimestamp(pb.start());
		                 res.End = Time::FromTimestamp(pb.end());
	                 },
	                 [&res] ( const pb::TagStatistics & pb) {
		                 res.TagStats.insert(std::make_pair(pb.id(),LoadStatistics(pb)));
	                 });
	return res;
}

void TagStatisticsCache::Save(const fs::path & tddAbsolutePath,
                              const TagStatisticsHelper::Timed & stats) {
	pb::TagStatisticsCacheHeader h;
	h.set_version(CACHE_VERSION);
	stats.Start.ToTimestamp(h.mutable_start());
	stats.End.ToTimestamp(h.mutable_end());
	std::vector<ReadWriter::LineWriter> lines;
	for ( const auto & [tagID,tagStats] : stats.TagStats ) {
		lines.push_back([tagStats = std::ref(tagStats)](pb::TagStatistics & line) {
			                SaveStatistics(&line,tagStats);
		                });
	}
	ReadWriter::Write(tddAbsolutePath / CACHE_PATH,
	                  h,
	                  lines);
}

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
