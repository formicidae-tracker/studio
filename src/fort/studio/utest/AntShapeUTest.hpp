#pragma once

#include <gtest/gtest.h>

#include <fort/studio/bridge/ExperimentBridge.hpp>


class AntShapeUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	ExperimentBridge d_experiment;
};
