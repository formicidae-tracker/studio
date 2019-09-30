#include "AntUTest.hpp"

#include "Ant.hpp"

using namespace fort::myrmidon;

TEST_F(AntUTest,IdentificationChecking) {
	auto md = new pb::AntMetadata();
	md->set_id(1);
	priv::Ant a(md);

	a.Identifications().push_back(priv::Ant::Identification());

	EXPECT_NO_THROW({a.SortAndCheckIdentifications();});

	a.Identifications().push_back(priv::Ant::Identification());

	EXPECT_THROW({
			a.SortAndCheckIdentifications();
		},priv::Ant::OverlappingIdentification);

	a.Identifications().back().End= std::make_shared<priv::FramePointer>();
	a.Identifications().front().Start= std::make_shared<priv::FramePointer>();
	a.Identifications().back().End->Path = "foo";
	a.Identifications().back().End->Frame = 1234;

	a.Identifications().front().Start->Frame = 1235;
	a.Identifications().front().Start->Path = "foo";

	EXPECT_NO_THROW({a.SortAndCheckIdentifications();});
}
