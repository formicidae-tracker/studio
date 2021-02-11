#pragma once

#include <gtest/gtest.h>

#include "Time.hpp"
#include <Eigen/Core>

#include <google/protobuf/message.h>

::testing::AssertionResult TimeEqual(const fort::myrmidon::Time & a,
                                     const fort::myrmidon::Time & b);

::testing::AssertionResult VectorAlmostEqual(const Eigen::Vector2d & a,
                                             const Eigen::Vector2d & b);

::testing::AssertionResult MessageEqual(const google::protobuf::Message &a,
                                        const google::protobuf::Message &b);
