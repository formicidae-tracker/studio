#include "DenseMapUTest.hpp"

#include "DenseMap.hpp"

#include <random>

#include <myrmidon/Time.hpp>


namespace fort {
namespace myrmidon {
namespace priv {

typedef DenseMap<uint32_t,uint32_t> DM;
typedef DenseMap<uint32_t,std::shared_ptr<uint32_t>> DMOfPtr;
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

TEST_F(DenseMapUTest,ConsistencyAndTiming) {
	std::random_device r;
	// Choose a random mean between 1 and 6
    std::default_random_engine e1(r());

    const static uint32_t SIZE = 2000;

    std::uniform_int_distribution<uint32_t> uniform(0, SIZE);

    std::set<uint32_t> keys;
    std::map<uint32_t,std::shared_ptr<uint32_t>> map;
    DMOfPtr denseMap;

    for (size_t i = 0; i < SIZE; ++i ) {
	    auto toInsert = std::make_pair(uniform(e1)+1,std::make_shared<uint32_t>(uniform(e1)));

	    auto mapRes = map.insert(toInsert);
	    auto denseMapRes = denseMap.insert(toInsert);
	    EXPECT_EQ(denseMapRes.second,mapRes.second);
	    keys.insert(toInsert.first);
    }

    EXPECT_EQ(denseMap.size(),map.size());

    // Check map are the same
    auto start = map.cbegin();
    auto denseStart = denseMap.cend();
    while (true) {
	    if ( start == map.cend() || denseStart == denseMap.cend() ) {
		    break;
	    }
	    EXPECT_EQ(start->first,denseStart->first);
	    EXPECT_EQ(start->second,denseStart->second);
	    EXPECT_EQ(*start->second,*denseStart->second);

	    ++start;++denseStart;
    }

    // Random access time is faster
    for ( const auto & k :keys ) {
	    auto start = Time::Now();
	    auto res = map.at(k);
	    auto middle = Time::Now();
	    auto denseRes = denseMap.at(k);
	    auto end = Time::Now();

	    EXPECT_TRUE(middle.Sub(start).Nanoseconds() >= end.Sub(middle).Nanoseconds())
		    << "map random access: " << middle.Sub(start)
		    << " DenseMap random access: " << end.Sub(middle);
    }



}


} // namespace priv
} // namespace myrmidon
} // namespace fort
