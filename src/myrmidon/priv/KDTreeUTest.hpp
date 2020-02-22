#pragma once

#include <gtest/gtest.h>

#include <myrmidon/Time.hpp>
#include "KDTree.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class KDTreeUTest : public ::testing::Test {
protected:
	typedef KDTree<int,double,2> KDT;

	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static std::vector<KDT::Element> elements;
	static std::set<std::pair<int,int>> collisions;
	static Duration N2Duration;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
