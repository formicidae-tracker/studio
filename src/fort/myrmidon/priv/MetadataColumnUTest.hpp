#pragma once

#include <gtest/gtest.h>

#include "MetadataColumn.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class MetadataColumnUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	MetadataColumn::Manager::Ptr manager;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
