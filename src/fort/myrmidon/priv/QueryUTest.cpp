#include "QueryUTest.hpp"


#include "Query.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

void QueryUTest::SetUp() {
	ASSERT_NO_THROW({
			experiment = Experiment::Create(TestSetup::Basedir() / "query.myrmidon");
			auto space = experiment->CreateSpace("box");
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir()));
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir()));
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",TestSetup::Basedir()));
		});

}

TEST_F(QueryUTest,TagStatistics) {
	TagStatistics::ByTagID tagStats;
	ASSERT_NO_THROW({
			Query::ComputeTagStatistics(experiment,
			                            tagStats);
		});

	EXPECT_EQ(tagStats.size(),1);
	ASSERT_EQ(tagStats.count(123),1);
	EXPECT_TRUE(TimeEqual(tagStats.at(123).FirstSeen,
	                      Time::Parse("2019-11-02T09:00:20.021Z")));

	EXPECT_TRUE(TimeEqual(tagStats.at(123).LastSeen,
	                      Time::Parse("2019-11-02T09:05:48.908406Z")));

	EXPECT_EQ(tagStats.at(123).ID,123);

	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::TOTAL_SEEN),3000);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::MULTIPLE_SEEN),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_500MS),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1S),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10S),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1M),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10M),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1H),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10H),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_MORE),0);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
