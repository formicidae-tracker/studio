#include "TimedMapUTest.hpp"

#include "TimeMap.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(TimedMapUTest,E2ETest) {
	typedef TimeMap<std::string,int> TM;

	TM map;

	map.Insert("foo",0,Time::SinceEver());
	map.Insert("foo",3,Time::FromTimeT(42));
	map.Insert("bar",6,Time::FromTimeT(42));

	EXPECT_THROW({
			map.Insert("doesnotmatter",42,Time::Forever());
		},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(map.At("foo",Time()),0);
			EXPECT_EQ(map.At("foo",Time::FromTimeT(42).Add(-1)),0);
			EXPECT_EQ(map.At("foo",Time::FromTimeT(42)),3);
			EXPECT_EQ(map.At("bar",Time::FromTimeT(42)),6);
		});

	EXPECT_THROW({
			map.At("baz",Time());
		},std::out_of_range);

	EXPECT_THROW({
			map.At("bar",Time::FromTimeT(42).Add(-1));
		},std::out_of_range);

	EXPECT_NO_THROW(map.Clear());

	EXPECT_THROW({
			map.At("bar",Time::FromTimeT(42));
		},std::out_of_range);

	EXPECT_THROW({
			map.At("foo",Time::FromTimeT(42));
		},std::out_of_range);


}

} // namespace priv
} // namespace myrmidon
} // namespace fort
