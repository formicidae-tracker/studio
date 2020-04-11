#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class ExperimentBridgeUTest : public ::testing::Test {
protected:
	void SetUp();

	fs::path pathExisting;
};
