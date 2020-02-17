#pragma once


#include <gtest/gtest.h>

#include <myrmidon/priv/TrackingDataDirectory.hpp>

class UniverseUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();

	static fort::myrmidon::priv::TrackingDataDirectory::ConstPtr s_foo[3];
};
