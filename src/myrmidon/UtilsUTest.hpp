#pragma once

#include <gtest/gtest.h>

#include "Time.hpp"
#include <Eigen/Core>

::testing::AssertionResult TimeEqual(const fort::myrmidon::Time & a,
                                     const fort::myrmidon::Time & b);

::testing::AssertionResult TimePtrEqual(const fort::myrmidon::Time::ConstPtr & a,
                                        const fort::myrmidon::Time::ConstPtr & b);

void ExpectAlmostEqualVector(const Eigen::Vector2d & a,
                             const Eigen::Vector2d & b);
