#include <iostream>

class TestObject;
std::ostream & operator<<(std::ostream & out, const TestObject & o );

#include "TimeValid.hpp"

#include "TimeValidUTest.hpp"


#include "../UtilsUTest.hpp"

using namespace fort::myrmidon;

class TestObject : public priv::TimeValid {
public:
	TestObject(const fort::Time & start, const fort::Time & end) {
		d_start = start;
		d_end = end;
	};
	std::string String() const {
		std::ostringstream os;
		os <<"TestObject{start:"
		   <<d_start
		   << ";end:"
		   << d_end
		   << "}";
		return os.str();
	}
};

std::ostream & operator<<(std::ostream & out, const TestObject & o ) {
	return out << o.String();
}

TEST_F(TimeValidUTest,HaveTimeValidity) {
	TestObject o(fort::Time::FromTimeT(1),
	             fort::Time::FromTimeT(10));

	EXPECT_FALSE(o.IsValid(fort::Time::FromTimeT(0)));
	EXPECT_TRUE(o.IsValid(fort::Time::FromTimeT(1)));
	EXPECT_TRUE(o.IsValid(fort::Time::FromTimeT(5)));
	EXPECT_TRUE(o.IsValid(fort::Time::FromTimeT(9)));
	EXPECT_TRUE(o.IsValid(fort::Time::FromTimeT(10).Add(-1 * fort::Duration::Nanosecond)));
	EXPECT_FALSE(o.IsValid(fort::Time::FromTimeT(10)));
	EXPECT_FALSE(o.IsValid(fort::Time::FromTimeT(11)));
}




TEST_F(TimeValidUTest,CanCheckOverlap) {

	struct TestData {
		std::vector<time_t> Times;
		size_t First,Second;

	};

	std::vector<TestData> data
		={
		  {{},0,0},
		  {{1,3,0,2},0,1},
		  {{1,3,0,1},0,0},
		  {{1,3,0,1,3,6,6,20,21,23},0,0},
		  {{1,3,0,1,3,6,6,20,21,23,22,300},4,5},
	};

	for(const auto & d : data ) {
		ASSERT_TRUE(d.Times.size() % 2 == 0);

		std::vector<std::shared_ptr<TestObject> > list;
		for(size_t i = 0; i < d.Times.size(); ++i) {
			auto s = fort::Time::FromTimeT(d.Times[i]);
			auto e = fort::Time::FromTimeT(d.Times[++i]);
			list.push_back(std::make_shared<TestObject>(s,e));
		}

		auto res = priv::TimeValid::SortAndCheckOverlap(list.begin(),list.end());

		if (d.First == d.Second) {
			EXPECT_EQ(res.first,res.second);
		} else {
			EXPECT_EQ(res.first-list.begin(),d.First);
			EXPECT_EQ(res.second-list.begin(),d.Second);
		}

	}
}



TEST_F(TimeValidUTest,CanGiveBoundaries) {

	std::vector<time_t> Times =
		{0,10,12,20,20,40,45,60};

	std::vector<std::shared_ptr<TestObject> > list;

	for(size_t i = 0; i < Times.size(); ++i) {
		auto s = fort::Time::FromTimeT(Times[i]);
		auto e = fort::Time::FromTimeT(Times[++i]);
		list.push_back(std::make_shared<TestObject>(s,e));
	}

	auto res = priv::TimeValid::SortAndCheckOverlap(list.begin(),list.end());
	ASSERT_TRUE(res.first == res.second);

	struct TestData {
		time_t T;
		fort::Time ExpectedLower;
		fort::Time ExpectedUpper;
	};

	std::vector<TestData> data
		= {
		   {-1,fort::Time::SinceEver(),fort::Time::FromTimeT(0)},
		   {
		    10,
		    fort::Time::FromTimeT(10),
		    fort::Time::FromTimeT(12)
		   },
		   {
		    11,
		    fort::Time::FromTimeT(10),
		    fort::Time::FromTimeT(12)
		   },
		   {
		    40,
		    fort::Time::FromTimeT(40),
		    fort::Time::FromTimeT(45)
		   },
		   {
		    42,
		    fort::Time::FromTimeT(40),
		    fort::Time::FromTimeT(45)
		   },
		   {
		    60,
		    fort::Time::FromTimeT(60),
		    fort::Time::Forever(),
		   },
		   {
		    65,
		    fort::Time::FromTimeT(60),
		    fort::Time::Forever(),
		   },

	};

	for(const auto & d : data ) {
		EXPECT_NO_THROW({
				auto t = fort::Time::FromTimeT(d.T);
				auto lower = priv::TimeValid::LowerUnvalidBound(t,list.begin(),list.end());
				auto upper = priv::TimeValid::UpperUnvalidBound(t,list.begin(),list.end());
				EXPECT_TRUE(TimeEqual(upper,d.ExpectedUpper));
				EXPECT_TRUE(TimeEqual(lower,d.ExpectedLower));
			});

	}

	std::vector<time_t> invalidTimes = {0,5,12,17,20,24,45,55};

	for(const auto & tt : invalidTimes) {
		auto t = fort::Time::FromTimeT(tt);
		EXPECT_THROW({
				::priv::TimeValid::LowerUnvalidBound(t,list.begin(),list.end());
			},std::invalid_argument) << " for time " << t;
		EXPECT_THROW({
				::priv::TimeValid::UpperUnvalidBound(t,list.begin(),list.end());
			},std::invalid_argument) << " for time " << t;

	}

}
