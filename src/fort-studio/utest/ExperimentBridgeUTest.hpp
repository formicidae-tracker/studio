#pragma once

#include <gtest/gtest.h>

#include <myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class ExperimentBridgeUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static fmp::Experiment::Ptr s_experiment;

};
