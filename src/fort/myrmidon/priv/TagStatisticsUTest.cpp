#include <fort/myrmidon/Time.hpp>

#include "TagStatisticsUTest.hpp"

#include "TagStatistics.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/UtilsUTest.hpp>

#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(TagStatisticsUTest,ComputeAndUpdatesGap) {

	struct TestData {
		Duration D;
		TagStatistics::CountHeader H;
	};

	std::vector<TestData> testdata
		= {
		   {-1,TagStatistics::CountHeader(0)},
		   {1,TagStatistics::GAP_500MS},
		   {499*Duration::Millisecond,TagStatistics::GAP_500MS},
		   {500*Duration::Millisecond,TagStatistics::GAP_1S},
		   {999*Duration::Millisecond,TagStatistics::GAP_1S},
		   {1*Duration::Second,TagStatistics::GAP_10S},
		   {9*Duration::Second,TagStatistics::GAP_10S},
		   {10*Duration::Second,TagStatistics::GAP_1M},
		   {59*Duration::Second,TagStatistics::GAP_1M},
		   {1*Duration::Minute,TagStatistics::GAP_10M},
		   {9*Duration::Minute,TagStatistics::GAP_10M},
		   {10*Duration::Minute,TagStatistics::GAP_1H},
		   {59*Duration::Minute,TagStatistics::GAP_1H},
		   {1*Duration::Hour,TagStatistics::GAP_10H},
		   {9*Duration::Hour,TagStatistics::GAP_10H},
		   {10*Duration::Hour,TagStatistics::GAP_MORE},
	};

	Time t;
	for ( const auto & d : testdata ) {
		auto stat = TagStatisticsHelper::Create(0,t);
		auto gap  = TagStatisticsHelper::ComputeGap(t,t.Add(d.D));
		EXPECT_EQ(gap,d.H) << " testing for " << d.D;
		TagStatisticsHelper::UpdateGaps(stat,t,t.Add(d.D));
		if ( d.H == 0 ) {
			EXPECT_EQ(stat.Counts.sum(),1);
		} else {
			EXPECT_EQ(stat.Counts(d.H),1);
		}
	}

}

::testing::AssertionResult StatsEqual(const TagStatistics & a,
                                      const TagStatistics & b) {

	if ( a.ID != b.ID ) {
		return ::testing::AssertionFailure() << "Mismatching a.ID=" << a.ID
		                                     << " and b.ID=" << b.ID;
	}
	auto firstSeenAssertion = TimeEqual(a.FirstSeen,b.FirstSeen);
	if ( !firstSeenAssertion ) {
		return firstSeenAssertion;
	}

	auto lastSeenAssertion = TimeEqual(a.LastSeen,b.LastSeen);
	if ( !lastSeenAssertion ) {
		return lastSeenAssertion;
	}

	if ( a.Counts != b.Counts ) {
		return ::testing::AssertionFailure() << "Counts mismatch: a=" <<a.Counts.transpose()
		                                     << " and b=" << b.Counts.transpose();
	}

	return ::testing::AssertionSuccess();
}

::testing::AssertionResult StatsByIDEqual(const TagStatistics::ByTagID & a,
                                          const TagStatistics::ByTagID & b) {

	if ( a.size() != b.size() ) {
		return ::testing::AssertionFailure() << "stats dimension mismatch a.size=" << a.size()
		                                     << " and b.size=" << b.size();
	}

	for ( const auto & [tagID,stats] : a ) {
		auto fi = b.find(tagID);
		if ( fi == b.end() ) {
			return ::testing::AssertionFailure() << "b is missing stats for " << tagID;
		}
		auto statAssertion = StatsEqual(stats,fi->second);
		if ( !statAssertion ) {
			return statAssertion;
		}
	}

	return ::testing::AssertionSuccess();
}

::testing::AssertionResult TimedEqual(const TagStatisticsHelper::Timed & a,
                                      const TagStatisticsHelper::Timed & b) {
	auto startAssertion = TimeEqual(a.Start,b.Start);
	if ( !startAssertion ) {
		return startAssertion;
	}
	auto endAssertion = TimeEqual(a.End,b.End);
	if ( !endAssertion ) {
		return endAssertion;
	}
	return StatsByIDEqual(a.TagStats,b.TagStats);
}

TEST_F(TagStatisticsUTest,CacheIsconsistent) {
	auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001", TestSetup::Basedir());

	auto loaders = tdd->StatisticsLoader();
	std::vector<TagStatisticsHelper::Timed> computeds,cacheds;
	computeds.reserve(loaders.size());
	cacheds.reserve(loaders.size());
	for ( auto & l : loaders ) {
		computeds.push_back(l());
	}

	for ( auto & l : loaders ) {
		cacheds.push_back(l());
	}

	for( size_t i = 0 ; i < loaders.size(); ++i ) {
		EXPECT_TRUE(TimedEqual(computeds[i],cacheds[i]));
	}
	std::vector<TagStatistics::ByTagID> merged;
	merged.push_back(TagStatisticsHelper::MergeTimed(computeds.begin(),computeds.end()).TagStats);
	merged.push_back(TagStatisticsHelper::MergeTimed(cacheds.begin(),cacheds.end()).TagStats);

	EXPECT_TRUE(StatsByIDEqual(merged[0],merged[1]));

	auto doubled = TagStatisticsHelper::MergeSpaced(merged.begin(),merged.end());

	for ( const auto & [tagID,stats] : doubled ) {
		auto fi = merged.front().find(tagID);
		ASSERT_FALSE(fi == merged.front().end());
		EXPECT_EQ(stats.ID,fi->second.ID);
		EXPECT_TRUE(TimeEqual(stats.FirstSeen,fi->second.FirstSeen));
		EXPECT_TRUE(TimeEqual(stats.LastSeen,fi->second.LastSeen));
	}




}



} // namespace priv
} // namespace myrmidon
} // namespace fort
