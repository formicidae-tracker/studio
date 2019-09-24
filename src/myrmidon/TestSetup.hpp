#pragma once

#include <gtest/gtest.h>

#include <filesystem>

class TestSetup : public ::testing::EmptyTestEventListener {
public:
	inline static const std::filesystem::path & Basedir() {
		return s_testdir;
	}

private:

	void OnTestProgramStart(const ::testing::UnitTest& /* unit_test */) override;

	void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override;

	void OnTestStart(const ::testing::TestInfo& test_info) override;

	// Called after a failed assertion or a SUCCEED() invocation.
	void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

	// Called after a test ends.
	void OnTestEnd(const ::testing::TestInfo& test_info) override;


	static std::filesystem::path s_testdir;

};  // class TestSetup
