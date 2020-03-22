#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/ForwardDeclaration.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class AntUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	AntShapeTypeContainerPtr shapeTypes;
	AntMetadataPtr           antMetadata;
	AntPtr                   ant;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
