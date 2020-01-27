#include "SegmentIndexerUTest.hpp"



namespace fm = fort::myrmidon;
using namespace fm::priv;


void SegmentIndexerUTest::SetUp() {
	for(size_t i = 0; i < 10; ++i) {
		std::ostringstream os;
		os << i;
		d_testdata.push_back(std::make_tuple(10*i+1,fm::Time::FromTimeT(10*i+1),os.str()));
	}

	EXPECT_NO_THROW({
			for(const auto & d : d_testdata) {
				d_si.Insert(std::get<0>(d),std::get<1>(d),std::get<2>(d));
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
		EXPECT_EQ(std::get<0>(res[i]),std::get<0>(d_testdata[i])) << " for segment " << i;
		EXPECT_TRUE(std::get<1>(res[i]).Equals(std::get<1>(d_testdata[i]))) << " for segment " << i;
		EXPECT_EQ(std::get<2>(res[i]),std::get<2>(d_testdata[i])) << " for segment " << i;
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
	EXPECT_NO_THROW(si.Insert(1,fm::Time::FromTimeT(1),"0"));
	EXPECT_NO_THROW(si.Insert(11,fm::Time::FromTimeT(11),"1"));
	EXPECT_THROW({si.Insert(21,fm::Time::FromTimeT(6),"2");},std::invalid_argument);
	EXPECT_THROW({si.Insert(6,fm::Time::FromTimeT(21),"2");},std::invalid_argument);
	// It is permitted to make two segment have the same end value
	EXPECT_NO_THROW(si.Insert(21,fm::Time::FromTimeT(21),"0"));

}
