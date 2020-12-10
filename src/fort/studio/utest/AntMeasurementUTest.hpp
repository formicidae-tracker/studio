#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/TagCloseUp.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>


class AntMeasurementUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	ExperimentBridge d_experiment;
	fmp::TagCloseUp::ConstPtr d_closeUps[4];
};
