#include "ExperimentUTest.hpp"

#include "Experiment.hpp"

#include "../TestSetup.hpp"

#include <fstream>

namespace fm= fort::myrmidon;
namespace fs = std::filesystem;
using namespace fm::priv;


void ReadAll(const fs::path & a, std::vector<uint8_t> & data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(),std::ios::binary);
	data =  std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});
}

TEST_F(ExperimentUTest,IOTest) {
	try{
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon" );
		auto tdd = e->TrackingDataPaths();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd["foo.0000"].Path,"foo.0000");
		ASSERT_EQ(e->Ants().size(),3);
		EXPECT_EQ(e->Ants().find(1)->second->ID(),1);
		EXPECT_EQ(e->Ants().find(2)->second->ID(),2);
		EXPECT_EQ(e->Ants().find(3)->second->ID(),3);

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
			ASSERT_EQ(newData[i],originalData[i]) << "At byte " << i;
		}
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

}

TEST_F(ExperimentUTest,AndsAreCreatedSequentially) {
	Experiment::Ptr e;

	try{
		e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon" );
		size_t startSize = e->Ants().size();
		auto ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ASSERT_EQ(e->Ants().size(),startSize + 3);

		e->DeleteAnt(2);
		e->DeleteAnt(5);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),2);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),5);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),7);

	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}





TEST_F(ExperimentUTest,TestNewTrackingDataDirectories) {
	try {
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		e->AddTrackingDataDirectory(TestSetup::Basedir()/"foo.0001");
	} catch( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}
