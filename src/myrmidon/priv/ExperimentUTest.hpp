#pragma once

#include <gtest/gtest.h>

#include <filesystem>

namespace fort {

namespace myrmidon {

namespace priv {

class ExperimentUTest : public ::testing::Test {
protected:

	static void SetUpTestCase();
	static void TearDownTestCase();

	static std::filesystem::path  s_testdir;

};


}

}

}
