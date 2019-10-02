#pragma once

#include <gtest/gtest.h>

#include "Identifier.hpp"
#include "Identification.hpp"
#include "Ant.hpp"

class IdentificationUTest : public ::testing::Test {
protected:

	void SetUp();

	fort::myrmidon::priv::Identifier::Ptr      d_identifier;
	fort::myrmidon::priv::Ant::Ptr             d_ant;
	fort::myrmidon::priv::Identification::List d_list;

};
