#pragma once

#include <gtest/gtest.h>

#include "TagStatistics.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class TagStatisticsUTest : public ::testing::Test {
protected:
};

::testing::AssertionResult TimedEqual(const TagStatisticsHelper::Timed & a,
                                      const TagStatisticsHelper::Timed & b);


} // namespace priv
} // namespace myrmidon
} // namespace fort
