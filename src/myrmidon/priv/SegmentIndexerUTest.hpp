#pragma once

#include <gtest/gtest.h>
#include "SegmentIndexer.hpp"


class SegmentIndexerUTest : public ::testing::Test {
	void SetUp();
protected:
	std::vector<fort::myrmidon::priv::SegmentIndexer::Segment> d_testdata;
	fort::myrmidon::priv::SegmentIndexer d_si;
};
