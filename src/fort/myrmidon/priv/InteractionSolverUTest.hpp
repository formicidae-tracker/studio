#pragma once

#include "InteractionSolver.hpp"

#include <gtest/gtest.h>

#include "Space.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class InteractionSolverUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();

	static InteractionFrame::ConstPtr NaiveInteractions();

	static IdentifiedFrame::ConstPtr  frame;
	static Space::Universe::Ptr       universe;
	static AntByID                    ants;
	static InteractionFrame::ConstPtr interactions;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
