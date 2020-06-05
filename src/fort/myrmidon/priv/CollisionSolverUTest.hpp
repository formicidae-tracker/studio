#pragma once

#include "CollisionSolver.hpp"

#include <gtest/gtest.h>

#include "Space.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class CollisionSolverUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();

	static CollisionFrame::ConstPtr NaiveCollisions();

	static IdentifiedFrame::Ptr      frame;
	static Space::Universe::Ptr      universe;
	static AntByID                   ants;
	static CollisionFrame::ConstPtr  collisions;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
