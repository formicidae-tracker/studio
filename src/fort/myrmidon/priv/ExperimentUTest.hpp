#pragma once

#include <gtest/gtest.h>

#include "ForwardDeclaration.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class ExperimentUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();


	ExperimentPtr e;
};


}

}

}
