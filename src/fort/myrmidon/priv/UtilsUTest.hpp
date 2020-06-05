#pragma once

#include <gtest/gtest.h>

#include "Circle.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"
#include "Types.hpp"

::testing::AssertionResult PolygonEqual(const fort::myrmidon::priv::Polygon &a,
                                        const fort::myrmidon::priv::Polygon &b);


::testing::AssertionResult CapsuleEqual(const fort::myrmidon::priv::Capsule &a,
                                        const fort::myrmidon::priv::Capsule &b);


::testing::AssertionResult CircleEqual(const fort::myrmidon::priv::Circle &a,
                                       const fort::myrmidon::priv::Circle &b);


::testing::AssertionResult ShapeEqual(const fort::myrmidon::priv::Shape::ConstPtr &a,
                                      const fort::myrmidon::priv::Shape::ConstPtr &b);

::testing::AssertionResult AntStaticValueEqual(const fort::myrmidon::AntStaticValue &a,
                                               const fort::myrmidon::AntStaticValue &b);

::testing::AssertionResult AABBAlmostEqual(const fort::myrmidon::priv::AABB & a,
                                           const fort::myrmidon::priv::AABB & b);
