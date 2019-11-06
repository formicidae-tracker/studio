#pragma once

#include <gtest/gtest.h>

#include "Time.hpp"


::testing::AssertionResult TimeEqual(const fort::myrmidon::Time & a,
                                     const fort::myrmidon::Time & b);

::testing::AssertionResult TimePtrEqual(const fort::myrmidon::Time::ConstPtr & a,
                                        const fort::myrmidon::Time::ConstPtr & b);
