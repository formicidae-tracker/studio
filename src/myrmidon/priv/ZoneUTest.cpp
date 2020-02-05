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
		 {"", true},
		 {"foo", false},
		 {"foo-bar", false},
		 {"foo/bar", true},
		 {"/foo", true},
		 {"foo/", true},
		};
	auto manager = std::make_shared<Zone::Manager>();

	auto good = Zone::Manager::Create(manager,"good");
	for (const auto & d : testdata) {
		if (d.Throws == true) {
			EXPECT_THROW({
					Zone::Manager::Create(manager,d.Name);
					;},std::invalid_argument) << "Testing " << d.Name;
			EXPECT_THROW({
					good->SetName(d.Name);
				},std::invalid_argument) << "Testing " << d.Name;
		} else {
			EXPECT_NO_THROW({
					good->SetName(d.Name);
					EXPECT_EQ(good->URI().generic_string(),
					          d.Name);
					good->SetName("good");
				}) << "Testing " << d.Name;

			EXPECT_NO_THROW({
					auto res = Zone::Manager::Create(manager,d.Name);
					EXPECT_EQ(res->URI().generic_string(),
					          d.Name);
				}) << "Testing " << d.Name;
		}
	}

	EXPECT_THROW({
			Zone::Manager::Create(manager,"good");
		}, std::invalid_argument);

	manager.reset();
	EXPECT_THROW({
			good->SetName("willcrash");
		},DeletedReference<Zone::Manager>);
}


TEST_F(ZoneUTest,CanHoldTDD) {
	auto foo0 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000", TestSetup::Basedir());
	auto foo1 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001", TestSetup::Basedir());
	auto foo2 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002", TestSetup::Basedir());

	auto manager = std::make_shared<Zone::Manager>();
	auto z = Zone::Manager::Create(manager,"foo");
	EXPECT_NO_THROW({
			z->AddTrackingDataDirectory(foo2);
			z->AddTrackingDataDirectory(foo1);
			z->AddTrackingDataDirectory(foo0);
		});
	ASSERT_EQ(z->TrackingDataDirectories().size(),3);

	// now they are sorted
	EXPECT_EQ(z->TrackingDataDirectories()[0],foo0);
	EXPECT_EQ(z->TrackingDataDirectories()[1],foo1);
	EXPECT_EQ(z->TrackingDataDirectories()[2],foo2);

	try {
		z->AddTrackingDataDirectory(foo0);
		ADD_FAILURE() << "Should have thrown Zone::TDDOverlap but nothing is thrown";
	} catch (const Zone::TDDOverlap & e) {
		EXPECT_EQ(e.A(),foo0);
		EXPECT_EQ(e.B(),foo0);
	} catch (...) {
		ADD_FAILURE() << "It have thrown something else";
	}

	EXPECT_NO_THROW({
			manager->DeleteTrackingDataDirectory(foo0->URI());
		});

	EXPECT_THROW({
			manager->DeleteTrackingDataDirectory(foo0->URI());
		},Zone::UnmanagedTrackingDataDirectory);

	EXPECT_THROW({
			// Still having some data
			manager->DeleteZone("foo");
		},std::runtime_error);

	EXPECT_THROW({
			// Still having some data
			manager->DeleteZone("bar");
		},std::invalid_argument);

	auto z2 = Zone::Manager::Create(manager,"bar");

	EXPECT_NO_THROW({
			//not used by any other zone
			z2->AddTrackingDataDirectory(foo0);
		});


	EXPECT_THROW({
			//used by z
			z2->AddTrackingDataDirectory(foo2);
		},std::runtime_error);


	EXPECT_NO_THROW({
			// removes data that is associated with foo
			manager->DeleteTrackingDataDirectory(foo0->URI());
			// removes the zone is OK now
			manager->DeleteZone("bar");
		});

}

} //namespace priv
} //namespace myrmidon
} //namespace fort
