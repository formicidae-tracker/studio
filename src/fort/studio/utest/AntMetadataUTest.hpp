#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class AntMetadataBridge;

class AntMetadataUTest : public ::testing::Test {
protected :
	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	AntMetadataBridge  * metadata;
};
