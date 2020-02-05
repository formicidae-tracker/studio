#include "ZoneUTest.hpp"

#include "Zone.hpp"

#include <myrmidon/TestSetup.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(ZoneUTest,NameCheck) {
	struct TestData {
		std::string Name;
		bool Throws;
	};

	std::vector<TestData> testdata =
		{
		 {"foo", false},
		 {"foo-bar", false},
		 {"foo/bar", true},
		 {"/foo", true},
		 {"foo/", true},
		};
	Zone good("good");
	for (const auto & d : testdata) {
		if (d.Throws == true) {
			EXPECT_THROW({Zone bad(d.Name);},std::invalid_argument) << "Testing " << d.Name;
			EXPECT_THROW({good.SetName(d.Name);},std::invalid_argument) << "Testing " << d.Name;
		} else {
			EXPECT_NO_THROW({
					Zone newGood(d.Name);
					EXPECT_EQ(newGood.URI().generic_string(),
					          d.Name);
				}) << "Testing " << d.Name;
			EXPECT_NO_THROW({
					good.SetName(d.Name);
					EXPECT_EQ(good.URI().generic_string(),
					          d.Name);
				}) << "Testing " << d.Name;
		}
	}
}


TEST_F(ZoneUTest,CanHoldTDD) {
	auto foo0 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000", TestSetup::Basedir());
	auto foo1 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001", TestSetup::Basedir());
	auto foo2 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002", TestSetup::Basedir());

	Zone z("foo");
	EXPECT_NO_THROW({
			z.AddTrackingDataDirectory(foo2);
			z.AddTrackingDataDirectory(foo1);
			z.AddTrackingDataDirectory(foo0);
		});
	ASSERT_EQ(z.TrackingDataDirectories().size(),3);

	// now they are sorted
	EXPECT_EQ(z.TrackingDataDirectories()[0],foo0);
	EXPECT_EQ(z.TrackingDataDirectories()[1],foo1);
	EXPECT_EQ(z.TrackingDataDirectories()[2],foo2);

	try {
		z.AddTrackingDataDirectory(foo0);
		ADD_FAILURE() << "Should have thrown Zone::TDDOverlap but nothing is thrown";
	} catch (const Zone::TDDOverlap & e) {
		EXPECT_EQ(e.A(),foo0);
		EXPECT_EQ(e.B(),foo0);
	} catch (...) {
		ADD_FAILURE() << "It have thrown something else";
	}

	EXPECT_NO_THROW({
			z.DeleteTrackingDataDirectory(foo0->URI());
		});

	EXPECT_THROW({
			z.DeleteTrackingDataDirectory(foo0->URI());
		},Zone::UnmanagedTrackingDataDirectory);

}

} //namespace priv
} //namespace myrmidon
} //namespace fort
