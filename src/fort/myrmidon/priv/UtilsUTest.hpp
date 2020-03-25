#pragma once

#include <gtest/gtest.h>

#include "Circle.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"
#include "Types.hpp"

::testing::AssertionResult PolygonEqual(const fort::myrmidon::priv::Polygon::ConstPtr &a,
                                        const fort::myrmidon::priv::Polygon::ConstPtr &b);


::testing::AssertionResult CapsuleEqual(const fort::myrmidon::priv::Capsule::ConstPtr &a,
                                        const fort::myrmidon::priv::Capsule::ConstPtr &b);


::testing::AssertionResult CircleEqual(const fort::myrmidon::priv::Circle::ConstPtr &a,
                                       const fort::myrmidon::priv::Circle::ConstPtr &b);


::testing::AssertionResult ShapeEqual(const fort::myrmidon::priv::Shape::ConstPtr &a,
                                      const fort::myrmidon::priv::Shape::ConstPtr &b);

::testing::AssertionResult AntStaticValueEqual(const fort::myrmidon::priv::AntStaticValue &a,
                                               const fort::myrmidon::priv::AntStaticValue &b);
