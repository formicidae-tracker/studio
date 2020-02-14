#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <myrmidon/TestSetup.hpp>


int main(int argc, char ** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);


	auto & listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
