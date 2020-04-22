#pragma once

#include "Zone.hpp"


#include <gtest/gtest.h>


namespace fort {
namespace myrmidon {
namespace priv {

class ZoneUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	std::vector<Shape::ConstPtr> shapes;
	Zone::Ptr                    zone;

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
