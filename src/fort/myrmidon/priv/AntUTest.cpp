#include "AntUTest.hpp"

#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


TEST_F(AntUTest,IdentificationChecking) {
	// priv::Ant a(1);

	// a.Identifications().push_back(priv::Ant::Identification());

	// EXPECT_NO_THROW({a.SortAndCheckIdentifications();});

	// a.Identifications().push_back(priv::Ant::Identification());

	// EXPECT_THROW({
	// 		a.SortAndCheckIdentifications();
	// 	},priv::Ant::OverlappingIdentification);

	// a.Identifications().back().End= std::make_shared<priv::FramePointer>();
	// a.Identifications().front().Start= std::make_shared<priv::FramePointer>();
	// a.Identifications().back().End->Path = "foo";
	// a.Identifications().back().End->Frame = 1234;

	// a.Identifications().front().Start->Frame = 1235;
	// a.Identifications().front().Start->Path = "foo";

	// EXPECT_NO_THROW({a.SortAndCheckIdentifications();});
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
