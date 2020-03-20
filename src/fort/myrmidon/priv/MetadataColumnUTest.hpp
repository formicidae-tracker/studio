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

	AntMetadata::UniqueColumnList::Ptr list;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
