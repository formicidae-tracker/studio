#include "ZoneUTest.hpp"

#include "Zone.hpp"

#include <myrmidon/TestSetup.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::ConstPtr ZoneUTest::s_foo[3];

void ZoneUTest::SetUpTestSuite() {
	s_foo[0] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",
	                                       TestSetup::Basedir());
	s_foo[1] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",
	                                       TestSetup::Basedir());
	s_foo[2] = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",
	                                       TestSetup::Basedir());
}

void ZoneUTest::TearDownTestSuite() {
	for (auto & tdd : s_foo) {
		tdd.reset();
	}
}


TEST_F(ZoneUTest,NameCheck) {
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
	auto group = std::make_shared<Zone::Group>();

	auto good = Zone::Group::Create(group,"good");
	for (const auto & d : testdata) {
		if (d.Throws == true) {
			EXPECT_THROW({
					Zone::Group::Create(group,d.Name);
					;},Zone::InvalidName) << "Testing " << d.Name;
			EXPECT_THROW({
					good->SetName(d.Name);
				},Zone::InvalidName) << "Testing " << d.Name;
		} else {
			EXPECT_NO_THROW({
					good->SetName(d.Name);
					EXPECT_EQ(good->URI().generic_string(),
					          d.Name);
					good->SetName("good");
				}) << "Testing " << d.Name;

			EXPECT_NO_THROW({
					auto res = Zone::Group::Create(group,d.Name);
					EXPECT_EQ(res->URI().generic_string(),
					          d.Name);
				}) << "Testing " << d.Name;
		}
	}

	EXPECT_THROW({
			Zone::Group::Create(group,"good");
		}, Zone::InvalidName);

	group.reset();
	EXPECT_THROW({
			good->SetName("willcrash");
		},DeletedReference<Zone::Group>);
}


TEST_F(ZoneUTest,CanHoldTDD) {

	auto group = std::make_shared<Zone::Group>();
	auto z = Zone::Group::Create(group,"foo");
	EXPECT_NO_THROW({
			z->AddTrackingDataDirectory(s_foo[2]);
			z->AddTrackingDataDirectory(s_foo[1]);
			z->AddTrackingDataDirectory(s_foo[0]);
		});
	ASSERT_EQ(z->TrackingDataDirectories().size(),3);

	// now they are sorted
	EXPECT_EQ(z->TrackingDataDirectories()[0],s_foo[0]);
	EXPECT_EQ(z->TrackingDataDirectories()[1],s_foo[1]);
	EXPECT_EQ(z->TrackingDataDirectories()[2],s_foo[2]);

	try {
		z->AddTrackingDataDirectory(s_foo[0]);
		ADD_FAILURE() << "Should have thrown Zone::TDDOverlap but nothing is thrown";
	} catch (const Zone::TDDOverlap & e) {
		EXPECT_EQ(e.A(),s_foo[0]);
		EXPECT_EQ(e.B(),s_foo[0]);
	} catch (...) {
		ADD_FAILURE() << "It have thrown something else";
	}

	EXPECT_NO_THROW({
			group->DeleteTrackingDataDirectory(s_foo[0]->URI());
		});

	EXPECT_THROW({
			group->DeleteTrackingDataDirectory(s_foo[0]->URI());
		},Zone::UnmanagedTrackingDataDirectory);

	EXPECT_THROW({
			// Still having some data
			group->DeleteZone("foo");
		},Zone::ZoneNotEmpty);

	EXPECT_THROW({
			group->DeleteZone("bar");
		},Zone::UnmanagedZone);

	auto z2 = Zone::Group::Create(group,"bar");

	EXPECT_NO_THROW({
			//not used by any other zone
			z2->AddTrackingDataDirectory(s_foo[0]);
		});


	EXPECT_THROW({
			//used by z
			z2->AddTrackingDataDirectory(s_foo[2]);
		},Zone::TDDAlreadyInUse);


	EXPECT_NO_THROW({
			// removes data that is associated with foo
			group->DeleteTrackingDataDirectory(s_foo[0]->URI());
			// removes the zone is OK now
			group->DeleteZone("bar");
		});

}

TEST_F(ZoneUTest,ExceptionFormatting) {

	struct TestData {
		std::runtime_error E;
		std::string What;
	};

	Zone::Group::Ptr group;
	Zone::Ptr z;

	ASSERT_NO_THROW({
			group = std::make_shared<Zone::Group>();
			z = Zone::Group::Create(group,"z");
			z->AddTrackingDataDirectory(s_foo[1]);
			z->AddTrackingDataDirectory(s_foo[0]);
		});

	std::vector<TestData> testdata =
		{
		 {
		  Zone::TDDOverlap(s_foo[0],s_foo[0]),
		  "TDD{URI:foo.0000, start:2019-11-02T09:00:20.021Z, end:2019-11-02T09:02:00.848126001Z} and TDD{URI:foo.0000, start:2019-11-02T09:00:20.021Z, end:2019-11-02T09:02:00.848126001Z} overlaps in time",
		 },
		 {
		  Zone::UnmanagedTrackingDataDirectory("doo"),
		  "TDD:'doo' is not managed by this Zone or Zone::Group",
		 },
		 {
		  Zone::UnmanagedZone("doo"),
		  "Zone:'doo' is not managed by this Zone::Group",
		 },
		 {
		  Zone::InvalidName("doh","it is 'doh'! Doh!"),
		  "Invalid Zone name 'doh': it is 'doh'! Doh!",
		 },
		 {
		  Zone::ZoneNotEmpty(*z),
		  "Zone:'z' is not empty (contains:{foo.0000,foo.0001})",
		 },
		 {
		  Zone::TDDAlreadyInUse("foo.0000","z"),
		  "TDD:'foo.0000' is in use in Zone:'z'",
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
