#include "SpaceUTest.hpp"

#include "Space.hpp"

#include <myrmidon/TestSetup.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::ConstPtr SpaceUTest::s_foo[3];

void SpaceUTest::SetUpTestSuite() {
	s_foo[0] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",
	                                       TestSetup::Basedir());
	s_foo[1] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",
	                                       TestSetup::Basedir());
	s_foo[2] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",
	                                       TestSetup::Basedir());
}

void SpaceUTest::TearDownTestSuite() {
	for (auto & tdd : s_foo) {
		tdd.reset();
	}
}


TEST_F(SpaceUTest,NameCheck) {
	struct TestData {
		std::string Name;
		bool Throws;
	};

	std::vector<TestData> testdata =
		{
		 {"", true},
		 {"foo", false},
		 {"foo-bar", false},
		 {"foo/bar", true},
		 {"/foo", true},
		 {"foo/", true},
		};
	auto universe = std::make_shared<Space::Universe>();

	auto good = Space::Universe::Create(universe,0,"good");
	for (const auto & d : testdata) {
		if (d.Throws == true) {
			EXPECT_THROW({
					Space::Universe::Create(universe,0,d.Name);
					;},Space::InvalidName) << "Testing " << d.Name;
			EXPECT_THROW({
					good->SetName(d.Name);
				},Space::InvalidName) << "Testing " << d.Name;
		} else {
			EXPECT_NO_THROW({
					good->SetName(d.Name);
					EXPECT_EQ(good->Name(),
					          d.Name);
					EXPECT_EQ(good->URI(),
					          "spaces/" + std::to_string(good->SpaceID()));
					good->SetName("good");
				}) << "Testing " << d.Name;

			EXPECT_NO_THROW({
					auto res = Space::Universe::Create(universe,0,d.Name);
					EXPECT_EQ(res->Name(),
					          d.Name);
					EXPECT_EQ(res->URI(),
					          "spaces/" + std::to_string(res->SpaceID()));
				}) << "Testing " << d.Name;
		}
	}

	EXPECT_THROW({
			Space::Universe::Create(universe,0,"good");
		}, Space::InvalidName);

	universe.reset();
	EXPECT_THROW({
			good->SetName("willcrash");
		},DeletedReference<Space::Universe>);
}


TEST_F(SpaceUTest,CanHoldTDD) {

	auto universe = std::make_shared<Space::Universe>();
	auto foo = Space::Universe::Create(universe,0,"foo");
	EXPECT_NO_THROW({
			foo->AddTrackingDataDirectory(s_foo[2]);
			foo->AddTrackingDataDirectory(s_foo[1]);
			foo->AddTrackingDataDirectory(s_foo[0]);
		});
	ASSERT_EQ(foo->TrackingDataDirectories().size(),3);

	// now they are sorted
	EXPECT_EQ(foo->TrackingDataDirectories()[0],s_foo[0]);
	EXPECT_EQ(foo->TrackingDataDirectories()[1],s_foo[1]);
	EXPECT_EQ(foo->TrackingDataDirectories()[2],s_foo[2]);

	try {
		foo->AddTrackingDataDirectory(s_foo[0]);
		ADD_FAILURE() << "Should have thrown Space::TDDOverlap but nothing is thrown";
	} catch (const Space::TDDOverlap & e) {
		EXPECT_EQ(e.A(),s_foo[0]);
		EXPECT_EQ(e.B(),s_foo[0]);
	} catch (...) {
		ADD_FAILURE() << "It have thrown something else";
	}

	EXPECT_NO_THROW({
			universe->DeleteTrackingDataDirectory(s_foo[0]->URI());
		});

	EXPECT_THROW({
			universe->DeleteTrackingDataDirectory(s_foo[0]->URI());
		},Space::UnmanagedTrackingDataDirectory);

	EXPECT_THROW({
			// Still having some data
			universe->DeleteSpace(foo->SpaceID());
		},Space::SpaceNotEmpty);

	EXPECT_THROW({
			universe->DeleteSpace(foo->SpaceID()+1);
		},Space::UnmanagedSpace);

	auto bar = Space::Universe::Create(universe,0,"bar");

	EXPECT_NO_THROW({
			//not used by any other zone
			bar->AddTrackingDataDirectory(s_foo[0]);
		});


	EXPECT_THROW({
			//used by foo
			bar->AddTrackingDataDirectory(s_foo[2]);
		},Space::TDDAlreadyInUse);


	EXPECT_NO_THROW({
			// removes data that is associated with foo
			universe->DeleteTrackingDataDirectory(s_foo[0]->URI());
			// removes the zone is OK now
			universe->DeleteSpace(bar->SpaceID());
		});

}

TEST_F(SpaceUTest,ExceptionFormatting) {
	struct TestData {
		std::runtime_error E;
		std::string What;
	};

	Space::Universe::Ptr universe;
	Space::Ptr z;

	ASSERT_NO_THROW({
			universe = std::make_shared<Space::Universe>();
			z = Space::Universe::Create(universe,0,"z");
			z->AddTrackingDataDirectory(s_foo[1]);
			z->AddTrackingDataDirectory(s_foo[0]);
		});

	std::vector<TestData> testdata =
		{
		 {
		  Space::TDDOverlap(s_foo[0],s_foo[0]),
		  "TDD{URI:'foo.0000', start:2019-11-02T09:00:20.021Z, end:2019-11-02T09:02:00.848126001Z} and TDD{URI:'foo.0000', start:2019-11-02T09:00:20.021Z, end:2019-11-02T09:02:00.848126001Z} overlaps in time",
		 },
		 {
		  Space::UnmanagedTrackingDataDirectory("doo"),
		  "TDD:'doo' is not managed by this Space or Universe",
		 },
		 {
		  Space::UnmanagedSpace("doo"),
		  "Space:'doo' is not managed by this Universe",
		 },
		 {
		  Space::InvalidName("doh","it is 'doh'! Doh!"),
		  "Invalid Space name 'doh': it is 'doh'! Doh!",
		 },
		 {
		  Space::SpaceNotEmpty(*z),
		  "Space:'z' is not empty (contains:{foo.0000,foo.0001})",
		 },
		 {
		  Space::TDDAlreadyInUse("foo.0000","z"),
		  "TDD:'foo.0000' is in use in Space:'z'",
		 },
		};
	for (const auto & d: testdata) {
		EXPECT_EQ(std::string(d.E.what()),
		          d.What);
	}

};

} //namespace priv
} //namespace myrmidon
} //namespace fort
