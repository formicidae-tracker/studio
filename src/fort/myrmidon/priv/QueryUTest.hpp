#pragma once

#include <gtest/gtest.h>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class QueryUTest : public ::testing::Test {
protected:
	Experiment::Ptr experiment;
	void SetUp();
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
