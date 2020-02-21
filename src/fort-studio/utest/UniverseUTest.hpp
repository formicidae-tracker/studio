#pragma once


#include <gtest/gtest.h>

#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <myrmidon/priv/Experiment.hpp>

class UniverseBridge;

class UniverseUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();

	void SetUp();
	void TearDown();

	static fort::myrmidon::priv::TrackingDataDirectory::ConstPtr s_foo[3];

	fort::myrmidon::priv::Experiment::Ptr experiment;
	UniverseBridge                      * universe;
};
