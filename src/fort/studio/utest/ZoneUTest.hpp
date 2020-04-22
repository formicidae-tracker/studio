#pragma once

#include <gtest/gtest.h>


#include <fort/studio/bridge/ZoneBridge.hpp>

class ZoneUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	ZoneBridge         * zones;
};
