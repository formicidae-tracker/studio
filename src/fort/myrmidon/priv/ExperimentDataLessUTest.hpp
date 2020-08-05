#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class ExperimentDataLessUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	fs::path experimentPath;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
