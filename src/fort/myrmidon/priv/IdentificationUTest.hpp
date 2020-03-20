#pragma once

#include <gtest/gtest.h>

#include "Identifier.hpp"
#include "Identification.hpp"
#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


class IdentificationUTest : public ::testing::Test {
protected:

	void SetUp();

	fort::myrmidon::priv::Identifier::Ptr      d_identifier;
	fort::myrmidon::priv::Ant::Ptr             d_ant;
	fort::myrmidon::priv::Identification::List d_list;
	AntShapeTypeContainerConstPtr              d_shapeTypes;
	AntMetadataConstPtr                        d_metadata;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
