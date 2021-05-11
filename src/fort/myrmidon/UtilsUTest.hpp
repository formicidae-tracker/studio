#pragma once

#include <gtest/gtest.h>

#include <fort/time/Time.hpp>
#include <Eigen/Core>

#include <google/protobuf/message.h>

::testing::AssertionResult TimeEqual(const fort::Time & a,
                                     const fort::Time & b);

::testing::AssertionResult VectorAlmostEqual(const Eigen::Vector2d & a,
                                             const Eigen::Vector2d & b);

::testing::AssertionResult MessageEqual(const google::protobuf::Message &a,
                                        const google::protobuf::Message &b);
