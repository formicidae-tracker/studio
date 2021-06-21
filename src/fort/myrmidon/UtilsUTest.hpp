#pragma once

#include <gtest/gtest.h>

#include <fort/time/Time.hpp>
#include <Eigen/Core>

#include <fort/myrmidon/Shapes.hpp>

#include <google/protobuf/message.h>

::testing::AssertionResult TimeEqual(const fort::Time & a,
                                     const fort::Time & b);

::testing::AssertionResult VectorAlmostEqual(const Eigen::Vector2d & a,
                                             const Eigen::Vector2d & b);

::testing::AssertionResult MessageEqual(const google::protobuf::Message &a,
                                        const google::protobuf::Message &b);

::testing::AssertionResult PolygonEqual(const fort::myrmidon::Polygon &a,
                                        const fort::myrmidon::Polygon &b);


::testing::AssertionResult CapsuleEqual(const fort::myrmidon::Capsule &a,
                                        const fort::myrmidon::Capsule &b);


::testing::AssertionResult CircleEqual(const fort::myrmidon::Circle &a,
                                       const fort::myrmidon::Circle &b);


::testing::AssertionResult ShapeEqual(const fort::myrmidon::Shape &a,
                                      const fort::myrmidon::Shape &b);

::testing::AssertionResult AntStaticValueEqual(const fort::myrmidon::AntStaticValue &a,
                                               const fort::myrmidon::AntStaticValue &b);

::testing::AssertionResult AABBAlmostEqual(const fort::myrmidon::AABB & a,
                                           const fort::myrmidon::AABB & B);
