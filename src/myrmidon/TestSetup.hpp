#pragma once

#include <gtest/gtest.h>

#include <myrmidon/utils/FileSystem.hpp>

#include "Time.hpp"

class TestSetup : public ::testing::EmptyTestEventListener {
public:
	inline static const fs::path & Basedir() {
		return s_testdir;
	}

	inline static const fort::myrmidon::Time StartTime(const fs::path & fs) {
		return s_times[fs].first;
	}

	inline static const fort::myrmidon::Time EndTime(const fs::path & fs) {
		return s_times[fs].second;
	}

	inline static const std::map<fs::path,std::shared_ptr<uint32_t> > CloseUpFilesForPath(const fs::path & p ) {
		return s_closeUpFiles[p];
	}

private:

	void OnTestProgramStart(const ::testing::UnitTest& /* unit_test */) override;

	void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override;

	void OnTestStart(const ::testing::TestInfo& test_info) override;

	// Called after a failed assertion or a SUCCEED() invocation.
	void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

	// Called after a test ends.
	void OnTestEnd(const ::testing::TestInfo& test_info) override;

	static void CreateSnapshotFiles(std::vector<uint64_t> bounds,
	                                const fs::path & basedir);

	static fs::path s_testdir;
	static std::map<fs::path,std::pair<fort::myrmidon::Time,fort::myrmidon::Time> > s_times;
	static std::map<fs::path,std::map<fs::path,std::shared_ptr<uint32_t> > > s_closeUpFiles;
};  // class TestSetup
