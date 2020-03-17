#include <gtest/gtest.h>

#include "TestSetup.hpp"


int main(int argc, char ** argv) {
	::testing::InitGoogleTest(&argc, argv);


	auto & listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
