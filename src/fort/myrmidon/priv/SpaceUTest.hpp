#pragma once

#include <gtest/gtest.h>

#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class SpaceUTest : public ::testing::Test {
protected:

	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static TrackingDataDirectory::ConstPtr s_foo[3];
};

} //namespace priv
} //namespace myrmidon
} //namespace fort
