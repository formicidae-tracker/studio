#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class IdentifierBridge;

class IdentifierUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	IdentifierBridge   * identifier;
};
