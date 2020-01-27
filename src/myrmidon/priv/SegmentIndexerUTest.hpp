#pragma once

#include <gtest/gtest.h>
#include "SegmentIndexer.hpp"


class SegmentIndexerUTest : public ::testing::Test {
	void SetUp();
protected:
	std::vector<fort::myrmidon::priv::SegmentIndexer<std::string>::Segment> d_testdata;
	fort::myrmidon::priv::SegmentIndexer<std::string> d_si;
};
