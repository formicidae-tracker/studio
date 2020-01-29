#include "SegmentIndexerUTest.hpp"



namespace fm = fort::myrmidon;
using namespace fm::priv;


void SegmentIndexerUTest::SetUp() {
	for(size_t i = 0; i < 10; ++i) {
		std::ostringstream os;
		os << i;
		d_testdata.push_back(std::make_pair(FrameReference("",10*i+1,fm::Time::FromTimeT(10*i+1)),os.str()));
	}

	EXPECT_NO_THROW({
			for(const auto & d : d_testdata) {
				d_si.Insert(d.first,d.second);
			}
		});

}

TEST_F(SegmentIndexerUTest,CanStoreAnIndex) {


	std::vector<SegmentIndexer<std::string>::Segment> res;
	EXPECT_NO_THROW({
			res = d_si.Segments();
		});

	ASSERT_EQ(res.size(),d_testdata.size());
	for(size_t i =0 ; i < res.size(); ++i ){
		EXPECT_EQ(res[i].first.ID(),d_testdata[i].first.ID()) << " for segment " << i;
		EXPECT_TRUE(res[i].first.Time().Equals(d_testdata[i].first.Time())) << " for segment " << i;
		EXPECT_EQ(res[i].second,d_testdata[i].second) << " for segment " << i;
	}

}


TEST_F(SegmentIndexerUTest,CanFindSegment) {
	struct TestData {
		uint64_t F;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {1,"0"},
		   {10,"0"},
		   {11,"1"},
		   {42,"4"},
		   {1001,"9"},
	};

	for(const auto & d : data) {
		std::string res;
		EXPECT_NO_THROW({
				res = d_si.Find(fm::Time::FromTimeT(d.F));
			});
		EXPECT_EQ(res,d.Expected);
		EXPECT_NO_THROW({
				res = d_si.Find(d.F);
			});
		EXPECT_EQ(res,d.Expected);
	}

	EXPECT_THROW({
			auto res = d_si.Find(0);
		},std::out_of_range);

	EXPECT_THROW({
			auto res = d_si.Find(fm::Time::FromTimeT(0));
		},std::out_of_range);

}


TEST_F(SegmentIndexerUTest,EnforceIncreasingInvariant) {
	SegmentIndexer<std::string> si;
	EXPECT_NO_THROW(si.Insert(FrameReference("",1,fm::Time::FromTimeT(1)),"0"));
	EXPECT_NO_THROW(si.Insert(FrameReference("",11,fm::Time::FromTimeT(11)),"1"));
	EXPECT_THROW({si.Insert(FrameReference("",21,fm::Time::FromTimeT(6)),"2");},std::invalid_argument);
	EXPECT_THROW({si.Insert(FrameReference("",6,fm::Time::FromTimeT(21)),"2");},std::invalid_argument);
	// It is permitted to make two segment have the same end value
	EXPECT_NO_THROW(si.Insert(FrameReference("",21,fm::Time::FromTimeT(21)),"0"));

}
