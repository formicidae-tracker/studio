#pragma once

#include <gtest/gtest.h>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>

class ExperimentBridge;

class TagCloseUpUTest : public testing::Test {
protected:

	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	ExperimentBridge   * bridge;
};
