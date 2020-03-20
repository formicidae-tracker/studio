#pragma once

#include <gtest/gtest.h>

#include "AntMetadata.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class AntMetadataUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	AntMetadataUniqueColumnList::Ptr list;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
