#include "DenseMapUTest.hpp"

#include "DenseMap.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

typedef DenseMap<uint32_t,uint32_t> DM;

TEST_F(DenseMapUTest,TestInsertion) {
	DM map;
	EXPECT_THROW({
			// could not insert object 0
			auto res = map.insert(std::make_pair(0,0));
		},std::invalid_argument);

	auto res = map.insert(std::make_pair(1,0));
	EXPECT_TRUE(res.second);
	EXPECT_EQ(res.first->first,1);
	EXPECT_EQ(res.first->second,0);
	EXPECT_EQ((*res.first).first,1);
	EXPECT_EQ((*res.first).second,0);

	res = map.insert(std::make_pair(1,100));
	EXPECT_FALSE(res.second);
	EXPECT_EQ(res.first->first,1);
	EXPECT_EQ(res.first->second,0);
	EXPECT_EQ((*res.first).first,1);
	EXPECT_EQ((*res.first).second,0);

	res = map.insert(std::make_pair(10,100));
	EXPECT_TRUE(res.second);
	EXPECT_EQ(res.first->first,10);
	EXPECT_EQ(res.first->second,100);
	EXPECT_EQ((*res.first).first,10);
	EXPECT_EQ((*res.first).second,100);
}

TEST_F(DenseMapUTest,IterationAndErase) {
	std::vector<uint32_t> values = {1,2,4,5,6,7,8,9,10,12};
	DM map;
	size_t i = 0;
	for ( const auto & v : values ) {
		++i;
		auto res = map.insert(std::make_pair(i,v));
		ASSERT_TRUE(res.second);
	}

	ASSERT_EQ(map.size(),values.size());
	i = 0;
	for ( const auto & [k,v] : map) {
		++i;
		EXPECT_EQ(k,i);
		EXPECT_EQ(v,values[i-1]);
	}

	for ( i = 0; i < values.size(); ++i) {
		auto erased = map.erase(i+1);
		EXPECT_EQ(erased,1);
		EXPECT_EQ(map.size(),values.size() - i - 1);
		size_t ii = i+1;
		for ( const auto & [k,v] : map) {
			++ii;
			EXPECT_EQ(k,ii);
			EXPECT_EQ(v,values[ii-1]);
		}
	}

}

TEST_F(DenseMapUTest,Find) {
	std::vector<uint32_t> values = {1,2,4,5,6,7,8,9,10,12};
	DM map;
	size_t i = 0;
	for ( const auto & v : values ) {
		++i;
		auto res = map.insert(std::make_pair(i,v));
		ASSERT_TRUE(res.second);
	}

	DM::const_iterator cfi = map.find(4);
	EXPECT_TRUE(cfi != map.end());
	EXPECT_EQ(cfi->second,5);

	DM::iterator fi = map.find(8);
	EXPECT_TRUE(fi != map.end());
	EXPECT_EQ(fi->second,9);

	DM::const_iterator cnull = map.find(89);
	EXPECT_TRUE(cnull == map.end());


}


} // namespace priv
} // namespace myrmidon
} // namespace fort
