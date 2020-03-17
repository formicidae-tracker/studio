#pragma once

#include <gtest/gtest.h>

#include <myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class AntShapeTypeBridge;

class AntShapeTypeUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	AntShapeTypeBridge * shapeTypes;
};
