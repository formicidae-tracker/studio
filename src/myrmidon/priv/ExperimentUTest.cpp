#include "ExperimentUTest.hpp"

#include "Experiment.hpp"
#include "Ant.hpp"
#include "../TestSetup.hpp"

#include <fstream>

namespace fm= fort::myrmidon;
using namespace fm::priv;


void ReadAll(const fs::path & a, std::vector<uint8_t> & data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(),std::ios::binary);
	data =  std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});
}


TEST_F(ExperimentUTest,CanAddTrackingDataDirectory) {
	try {
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		google::protobuf::Timestamp startG,endG;
		ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:01:18.021-05:00",&startG),true);
		ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:01:20.021-05:00",&endG),true);
		auto start = Time::FromTimestamp(startG);
		auto end = Time::FromTimestamp(endG);
		TrackingDataDirectory tdd("bar",
		                          "baz",
		                          9,
		                          11,
		                          start,
		                          end,
		                          std::make_shared<SegmentIndexer<std::string>>(),
		                          MovieSegment::List());

		e->AddTrackingDataDirectory(tdd);

		ASSERT_EQ(e->TrackingDataDirectories().size(),2);
		e->Save(TestSetup::Basedir() / "test3.myrmidon");
		auto ee = Experiment::Open(TestSetup::Basedir() / "test3.myrmidon");

		ASSERT_EQ(ee->TrackingDataDirectories().size(),2);


	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentUTest,IOTest) {
	try{
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon" );
		auto tdd = e->TrackingDataDirectories();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd["foo.0000"].URI(),"foo.0000");
		ASSERT_EQ(tdd["foo.0000"].AbsoluteFilePath(),TestSetup::Basedir() / "foo.0000");
		ASSERT_EQ(e->ConstIdentifier().Ants().size(),3);
		EXPECT_EQ(e->ConstIdentifier().Ants().find(1)->second->ID(),1);
		EXPECT_EQ(e->ConstIdentifier().Ants().find(2)->second->ID(),2);
		EXPECT_EQ(e->ConstIdentifier().Ants().find(3)->second->ID(),3);
		EXPECT_EQ(e->AbsoluteFilePath(),TestSetup::Basedir() / "test.myrmidon");
		EXPECT_EQ(e->Basedir(), TestSetup::Basedir());

		EXPECT_EQ(e->Name(),"myrmidon test data");
		EXPECT_EQ(e->Author(),"myrmidon-tests");
		EXPECT_EQ(e->Comment(),"automatically generated data");
		EXPECT_EQ(e->Threshold(),42);
		EXPECT_EQ(e->Family(),fort::tags::Family::Tag16h5);

		e->Save(TestSetup::Basedir() / "test2.myrmidon");
	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	try {
		std::vector<uint8_t> originalData,newData;
		ReadAll( TestSetup::Basedir() /"test.myrmidon",originalData);
		ReadAll( TestSetup::Basedir() /"test2.myrmidon",newData);
		ASSERT_EQ(newData.size(),originalData.size());
		for(size_t i = 0; i < newData.size(); ++i) {
			ASSERT_EQ(newData[i],originalData[i]) << "At byte " << i << " over " << originalData.size();
		}
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

}



TEST_F(ExperimentUTest,TestNewTrackingDataDirectories) {
	try {
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		e->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir()/"foo.0001",TestSetup::Basedir()));
	} catch( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}
