#include "ExperimentUTest.hpp"

#include "Experiment.hpp"
#include "Ant.hpp"
#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>
#include <fort/myrmidon/priv/AntShapeType.hpp>
#include <fort/myrmidon/UtilsUTest.hpp>
#include <fstream>

namespace fort {
namespace myrmidon {
namespace priv {

void ExperimentUTest::SetUp() {
	e = Experiment::Create(TestSetup::Basedir() / "experiment-utest.myrmidon");
}

void ExperimentUTest::TearDown() {
	e.reset();
}


typedef AlmostContiguousIDContainer<fort::myrmidon::Ant::ID,Ant> Container;

void ReadAll(const fs::path & a, std::vector<uint8_t> & data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(),std::ios::binary);
	data =  std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});
}

TEST_F(ExperimentUTest,ExclusiveOpening) {
	auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
	EXPECT_THROW({
			auto b = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		},std::exception);

	EXPECT_THROW({
			auto b = Experiment::OpenReadOnly(TestSetup::Basedir() / "test.myrmidon");
		},std::exception);

	e.reset();
	auto ce = Experiment::OpenReadOnly(TestSetup::Basedir() / "test.myrmidon");
	EXPECT_NO_THROW({
			auto cb = Experiment::OpenReadOnly(TestSetup::Basedir() / "test.myrmidon");
		});

	EXPECT_THROW({
			auto b = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		},std::exception);
}


TEST_F(ExperimentUTest,CanAddTrackingDataDirectory) {
	try {
		e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002", TestSetup::Basedir());

		ASSERT_FALSE(e->Spaces().empty());
		e->AddTrackingDataDirectory(e->Spaces().begin()->second,tdd);

		ASSERT_EQ(e->Spaces().begin()->second->TrackingDataDirectories().size(),2);
		e->Save(TestSetup::Basedir() / "test3.myrmidon");
		e.reset();
		auto ee = Experiment::Open(TestSetup::Basedir() / "test3.myrmidon");

		ASSERT_FALSE(ee->Spaces().empty());
		ASSERT_EQ(ee->Spaces().begin()->second->TrackingDataDirectories().size(),2);


	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentUTest,IOTest) {
	try{
		e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon" );
		ASSERT_FALSE(e->Spaces().empty());
		auto tdd = e->Spaces().begin()->second->TrackingDataDirectories();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd[0]->URI(),"foo.0000");
		ASSERT_EQ(tdd[0]->AbsoluteFilePath(),TestSetup::Basedir() / "foo.0000");
		ASSERT_EQ(e->CIdentifier().CAnts().size(),3);
		EXPECT_EQ(e->CIdentifier().CAnts().find(1)->second->AntID(),1);
		EXPECT_EQ(e->CIdentifier().CAnts().find(2)->second->AntID(),2);
		EXPECT_EQ(e->CIdentifier().CAnts().find(3)->second->AntID(),3);
		EXPECT_EQ(e->AbsoluteFilePath(),TestSetup::Basedir() / "test.myrmidon");
		EXPECT_EQ(e->Basedir(), TestSetup::Basedir());

		EXPECT_EQ(e->Name(),"myrmidon test data");
		EXPECT_EQ(e->Author(),"myrmidon-tests");
		EXPECT_EQ(e->Comment(),"automatically generated data");
		EXPECT_EQ(e->Family(),fort::tags::Family::Tag36h11);

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


void ListAllMeasurements(const Experiment::MeasurementByTagCloseUp & measurements,
                         std::vector<Measurement::ConstPtr> & result) {
	size_t size = 0;
	for (const auto & [uri,measurementsByType] : measurements) {
		size += measurementsByType.size();
	}
	result.clear();
	result.reserve(size);
	for (const auto & [uri,measurementsByType] : measurements) {
		for (const auto & [type,m] : measurementsByType ) {
			result.push_back(m);
		}
	}
}

TEST_F(ExperimentUTest,MeasurementEndToEnd) {
	TrackingDataDirectory::Ptr foo0,foo1;
	Space::Ptr s;
	ASSERT_NO_THROW({
			e = Experiment::NewFile(TestSetup::Basedir() / "new-file.myrmidon");
			foo0 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
			foo1 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir());
			s = e->CreateSpace("box");
			e->AddTrackingDataDirectory(s,foo0);
			e->AddTrackingDataDirectory(s,foo1);
		});

	// It has a default measurment type Measurement::HEAD_TAIL_TYPE called "head-tail"
	EXPECT_EQ(e->MeasurementTypes().size(),1);
	if (e->MeasurementTypes().empty() == false) {
		auto defType = e->MeasurementTypes().begin()->second;
		EXPECT_EQ(defType->MTID(),
		          Measurement::HEAD_TAIL_TYPE);
		EXPECT_EQ(defType->Name(),
		          "head-tail");
	}

	EXPECT_THROW({
			//we can't create a new one with the same type
			e->CreateMeasurementType("foo",Measurement::HEAD_TAIL_TYPE);
		},std::runtime_error);

	EXPECT_THROW({
			//we can't delete the default one
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE);
		},std::invalid_argument);

	EXPECT_THROW({
			//we can't delete an inexistant one
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE+1);
		},std::runtime_error);


	EXPECT_THROW({
			//We are not allowed to add a measurement with an inexisting Type
			e->SetMeasurement(std::make_shared<Measurement>("foo.0000",
			                                                Measurement::HEAD_TAIL_TYPE+1,
			                                                Eigen::Vector2d(12,1),
			                                                Eigen::Vector2d(1,12),
			                                                12.0));
		},std::runtime_error);

	EXPECT_NO_THROW({
			e->CreateMeasurementType("foo");
		});

	EXPECT_NO_THROW({
			// its ok to be clumsy and use the same names for different type
			e->CreateMeasurementType("foo");
		});


	auto tcuPath = fs::path(foo0->URI()) / "frames" / std::to_string(foo0->StartFrame()) / "closeups/0x015";
	auto badPath = fs::path("bar.0000") / "frames" / std::to_string(foo0->StartFrame()) / "closeups/0x015";

	auto goodCustom = std::make_shared<Measurement>(tcuPath.generic_string(),
	                                                Measurement::HEAD_TAIL_TYPE+1,
	                                                Eigen::Vector2d(12,1),
	                                                Eigen::Vector2d(1,12),
	                                                12.0);
	auto goodDefault = std::make_shared<Measurement>(tcuPath.generic_string(),
	                                                 Measurement::HEAD_TAIL_TYPE,
	                                                 Eigen::Vector2d(12,12),
	                                                 Eigen::Vector2d(10,12),
	                                                 12.0);
	auto defaultWithBadPath = std::make_shared<Measurement>(badPath.generic_string(),
	                                                        Measurement::HEAD_TAIL_TYPE,
	                                                        Eigen::Vector2d(12,12),
	                                                        Eigen::Vector2d(10,12),
	                                                        12.0);

	EXPECT_NO_THROW({
			e->SetMeasurement(goodDefault);
			e->SetMeasurement(goodCustom);
		});

	EXPECT_THROW({
			e->SetMeasurement(defaultWithBadPath);
		},std::invalid_argument);

	std::vector<Measurement::ConstPtr> list = {goodCustom, goodDefault, defaultWithBadPath} ;
	ListAllMeasurements(e->Measurements(),list);
	EXPECT_EQ(list.size(),2);
	auto listContains = [&list](const Measurement::ConstPtr & m) {
		                    auto fi = std::find_if(list.cbegin(),list.cend(),
		                                           [&m](const Measurement::ConstPtr & it) -> bool {
			                                           return it->URI() == m->URI();
		                                           });
		                    return fi != list.cend();
	                    };
	EXPECT_TRUE(listContains(goodCustom));
	EXPECT_TRUE(listContains(goodDefault));
	EXPECT_FALSE(listContains(defaultWithBadPath));

	auto antBefore = e->CreateAnt();
	auto identBefore1 = Identifier::AddIdentification(e->Identifier(),
	                                                  antBefore->AntID(),
	                                                  1,
	                                                  Time::ConstPtr(),
	                                                  std::make_shared<Time>(foo0->EndDate()));
	identBefore1->SetTagSize(2.0);

	auto identBefore2 = Identifier::AddIdentification(e->Identifier(),
	                                                  antBefore->AntID(),
	                                                  0,
	                                                  std::make_shared<Time>(foo1->StartDate()),
	                                                  Time::ConstPtr());
	identBefore2->SetTagSize(2.0);


	struct MData {
		TrackingDataDirectory::Ptr TDD;
		size_t                     Offset;
		TagID                      TID;
		MeasurementType::ID        MTID;
	};
	std::vector<MData> mData =
		{
		 {foo0,0,0,1},
		 {foo0,0,0,2},
		 {foo0,0,1,1},
		 {foo0,0,1,2},
		 {foo0,1,0,1},
		 {foo0,1,0,2},
		 {foo0,1,1,1},
		 {foo0,1,1,2},
		 {foo1,0,0,1},
		 {foo1,0,0,2},
		 {foo1,0,1,1},
		 {foo1,0,1,2},
		 {foo1,1,0,1},
		 {foo1,1,0,2},
		 {foo1,1,1,1},
		 {foo1,1,1,2}
		};
	std::vector<std::string> paths;
	paths.reserve(mData.size());



	for ( const auto & md : mData ) {
		auto tcuPath = fs::path(md.TDD->URI())
			/ "frames"
			/ std::to_string(md.TDD->StartFrame() + md.Offset)
			/ "closeups"
			/ FormatTagID(md.TID);

		auto m = std::make_shared<Measurement>(tcuPath.generic_string(),
		                                       md.MTID,
		                                       Eigen::Vector2d(12,0),
		                                       Eigen::Vector2d(0,0),
		                                       1.0);
		paths.push_back(m->URI());
		ASSERT_NO_THROW(e->SetMeasurement(m));
	}

	//Now we add a super Ant
	auto antAfter = e->CreateAnt();
	auto identAfter1 = Identifier::AddIdentification(e->Identifier(),
	                                                 antAfter->AntID(),
	                                                 0,
	                                                 Time::ConstPtr(),
	                                                 std::make_shared<Time>(foo0->EndDate()));



	auto identAfter2 = Identifier::AddIdentification(e->Identifier(),
	                                                 antAfter->AntID(),
	                                                 1,
	                                                 std::make_shared<Time>(foo1->StartDate()),
	                                                 Time::ConstPtr());
	e->SetDefaultTagSize(1.0);
	EXPECT_TRUE(VectorAlmostEqual(identAfter1->AntPosition(),
	                              Eigen::Vector2d(6.0,0.0)));

	EXPECT_FALSE(identAfter1->HasUserDefinedAntPose());
	identAfter1->SetUserDefinedAntPose(Eigen::Vector2d(2,3),0.13);
	EXPECT_TRUE(identAfter1->HasUserDefinedAntPose());
	EXPECT_TRUE(VectorAlmostEqual(identAfter1->AntPosition(),
	                        Eigen::Vector2d(2,3)));
	EXPECT_EQ(identAfter1->AntAngle(),0.13);
	identAfter1->ClearUserDefinedAntPose();
	EXPECT_TRUE(VectorAlmostEqual(identAfter1->AntPosition(),
	                              Eigen::Vector2d(6.0,0.0)));

	EXPECT_FALSE(identAfter1->HasUserDefinedAntPose());



	std::vector<ComputedMeasurement> measurements;
	e->ComputeMeasurementsForAnt(measurements,
	                             antAfter->AntID(),
	                             1);

	EXPECT_EQ(measurements.size(), 4);
	for(const auto & m : measurements) {
		EXPECT_DOUBLE_EQ(9.6,m.LengthMM);
	}

	EXPECT_TRUE(VectorAlmostEqual(identBefore1->AntPosition(),
	                              Eigen::Vector2d(6.0,0.0)));



	e->ComputeMeasurementsForAnt(measurements,
	                             antBefore->AntID(),
	                             1);

	EXPECT_EQ(measurements.size(), 4);
	for(const auto & m : measurements) {
		EXPECT_DOUBLE_EQ(19.2,m.LengthMM);
	}

	EXPECT_THROW({
			e->ComputeMeasurementsForAnt(measurements,
			                             antAfter->AntID() + 100,
			                             1);
		},Container::UnmanagedObject);


	auto antLast = e->CreateAnt();
	Identifier::AddIdentification(e->Identifier(),
	                              antLast->AntID(),
	                              22,
	                              Time::ConstPtr(),
	                              Time::ConstPtr());

	e->ComputeMeasurementsForAnt(measurements,
	                             antAfter->AntID(),
	                             4);
	EXPECT_EQ(measurements.size(),0);

	e->ComputeMeasurementsForAnt(measurements,
	                             antLast->AntID(),
	                             1);
	EXPECT_EQ(measurements.size(),0);


	for ( const auto & uri : paths ) {
		ASSERT_NO_THROW(e->DeleteMeasurement(uri));
	}
	//deleting all measurements set the position to 0

	EXPECT_TRUE(VectorAlmostEqual(identBefore1->AntPosition(),
	                              Eigen::Vector2d(0.0,0.0)));
	EXPECT_TRUE(VectorAlmostEqual(identAfter1->AntPosition(),
	                              Eigen::Vector2d(0.0,0.0)));

	EXPECT_THROW({
			e->DeleteMeasurement("none/frames/23/closeups/0x01a/measurements/1");
		},std::invalid_argument);

	EXPECT_THROW({
			e->DeleteMeasurement("foo.0000/frames/0/closeups/0x01a/measurements/1");
		},std::runtime_error);

	EXPECT_THROW({
			e->DeleteMeasurement("foo.0000/frames/0/closeups/0x015/measurements/34");
		},std::runtime_error);

	EXPECT_NO_THROW({
			//OK it has no measurement
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE+2);
		});

	EXPECT_THROW({
			// contains a tracking data directory
			e->DeleteSpace(s->SpaceID());
		},std::runtime_error);

	EXPECT_THROW({
			// contains 2 measurements
			e->DeleteTrackingDataDirectory(foo0->URI());
		},std::runtime_error);


	EXPECT_THROW({
			//It contains data !!
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE+1);
		},std::runtime_error);


	EXPECT_NO_THROW({
			e->DeleteMeasurement(goodCustom->URI());
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE+1);
		});

	ListAllMeasurements(e->Measurements(),list);
	EXPECT_EQ(list.size(),1);
	EXPECT_TRUE(listContains(goodDefault));


	EXPECT_NO_THROW({
			e->DeleteMeasurement(goodDefault->URI());
		});

	ListAllMeasurements(e->Measurements(),list);
	EXPECT_EQ(list.size(),0);

	EXPECT_NO_THROW({
			e->DeleteTrackingDataDirectory(foo0->URI());
			e->DeleteTrackingDataDirectory(foo1->URI());
		});

	EXPECT_NO_THROW({
			e->DeleteSpace(s->SpaceID());
		});



}

TEST_F(ExperimentUTest,TooSmallHeadTailMeasurementAreNotPermitted) {
	TrackingDataDirectory::Ptr foo0;
	Space::Ptr s;
	ASSERT_NO_THROW({
			e = Experiment::NewFile(TestSetup::Basedir() / "small-head-tail-measurement-failure.myrmidon");
			foo0 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
			s = e->CreateSpace("box");
			e->AddTrackingDataDirectory(s,foo0);
			e->SetDefaultTagSize(1.0);
		});

	auto ant = e->CreateAnt();
	auto ident = Identifier::AddIdentification(e->Identifier(),
	                                           ant->AntID(),
	                                           1,
	                                           Time::ConstPtr(),
	                                           Time::ConstPtr());


	auto tcuPath = fs::path(foo0->URI())
			/ "frames"
			/ std::to_string(foo0->StartFrame() + 42)
			/ "closeups"
			/ FormatTagID(1);
	// this measurement is subpixel value, it should throw an exception when set to an experiment
	auto m = std::make_shared<Measurement>(tcuPath.generic_string(),
	                                       1,
	                                       Eigen::Vector2d(0.5,0),
	                                       Eigen::Vector2d(0,0),
	                                       1.0);
	ComputedMeasurement::List lengths;
	ASSERT_NO_THROW({
			e->ComputeMeasurementsForAnt(lengths,1,1);
		});
	ASSERT_EQ(lengths.size(),0);
	EXPECT_THROW({
			e->SetMeasurement(m);
		},std::invalid_argument);
	ASSERT_NO_THROW({
			e->ComputeMeasurementsForAnt(lengths,1,1);
		});
	// measurement should not have been stored as it fails
	EXPECT_EQ(lengths.size(),0);

	m = std::make_shared<Measurement>(tcuPath.generic_string(),
	                                  1,
	                                  Eigen::Vector2d(20,0),
	                                  Eigen::Vector2d(0,0),
	                                  1.0);
	ASSERT_NO_THROW({
			e->SetMeasurement(m);
		});
	ASSERT_NO_THROW({
			e->ComputeMeasurementsForAnt(lengths,1,1);
		});
	ASSERT_EQ(lengths.size(),1);
	auto antPosition = ident->AntPosition();
	auto antAngle = ident->AntAngle();

	m = std::make_shared<Measurement>(tcuPath.generic_string(),
	                                  1,
	                                  Eigen::Vector2d(0.5,0),
	                                  Eigen::Vector2d(0,0),
	                                  1.0);
	EXPECT_THROW({
			e->SetMeasurement(m);
		},std::invalid_argument);
	ASSERT_NO_THROW({
			e->ComputeMeasurementsForAnt(lengths,1,1);
		});
	// old measurment should have been kept;
	EXPECT_EQ(lengths.size(),1);
	EXPECT_EQ(antPosition,ident->AntPosition());
	EXPECT_EQ(antAngle,ident->AntAngle());
}


TEST_F(ExperimentUTest,CornerWidthRatioForFamilies) {
	struct TestData {
		tags::Family F;
	};

	std::vector<TestData> testdata =
		{
		 {tags::Family::Tag36h11},
		 {tags::Family::Tag36h10},
		 {tags::Family::Tag16h5},
		 {tags::Family::Tag25h9},
		 {tags::Family::Circle21h7},
		 {tags::Family::Circle49h12},
		 {tags::Family::Custom48h12},
		 {tags::Family::Standard41h12},
		 {tags::Family::Standard52h13},
		};

	for (const auto & d : testdata) {
		EXPECT_NO_THROW({
				double ratio = Experiment::CornerWidthRatio(d.F);
				EXPECT_TRUE( ratio < 1.0 && ratio > 0.0);
				//test internal caching of the value
				EXPECT_EQ(ratio,Experiment::CornerWidthRatio(d.F));
			});
	}

	EXPECT_EQ(Experiment::CornerWidthRatio(tags::Family::Tag36ARTag),1.0);


	EXPECT_THROW({
			Experiment::CornerWidthRatio(tags::Family::Undefined);
		},std::invalid_argument);



}

TEST_F(ExperimentUTest,AntMetadataManipulation) {
	auto alive = e->AddAntMetadataColumn("alive",AntMetadata::Type::BOOL);
	auto group = e->AddAntMetadataColumn("group",AntMetadata::Type::STRING);
	auto ant = e->CreateAnt();
	ant->SetValue("group",std::string("nurse"),Time::ConstPtr());
	//should throw because ant has a value
	EXPECT_THROW(group->SetMetadataType(AntMetadata::Type::INT),std::runtime_error);
	//OK to change a column without any values
	EXPECT_NO_THROW(alive->SetMetadataType(AntMetadata::Type::INT));
	// Adding a column marks adds a default value to all Ant immediatly
	auto ageInDays = e->AddAntMetadataColumn("age",AntMetadata::Type::DOUBLE);
	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<double>(ant->GetValue("age",Time())),0.0);
		});
	// always possible to change the column name, even if there are existing values
	EXPECT_NO_THROW({
			ageInDays->SetName("age-in-days");
			group->SetName("social-group");
		});
	EXPECT_THROW(ant->GetValue("group",Time()),std::out_of_range);
	EXPECT_THROW(ant->GetValue("age",Time()),std::out_of_range);
	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<std::string>(ant->GetValue("social-group",Time())),"nurse");
			EXPECT_EQ(std::get<double>(ant->GetValue("age-in-days",Time())),0.0);
		});

	EXPECT_THROW(e->DeleteAntMetadataColumn("social-group"),std::runtime_error);
	EXPECT_NO_THROW(e->DeleteAntMetadataColumn("age-in-days"));
	EXPECT_THROW(ant->GetValue("age-in-days",Time()),std::out_of_range);

}

TEST_F(ExperimentUTest,OldFilesAreOpenable) {
	EXPECT_NO_THROW({
			Experiment::OpenReadOnly(TestSetup::Basedir() / "test-0.1.myrmidon");
		});
}

TEST_F(ExperimentUTest,WillNotOpenFileQhichAreTooRecent) {
	auto path = TestSetup::Basedir() / "test-future.myrmidon";

	try {
		Experiment::OpenReadOnly(path);
		ADD_FAILURE() << "Opening " << path << " should have thrown a std::runtime_error";
	} catch ( const std::runtime_error & e) {
		std::ostringstream expected;
		expected << "Unexpected myrmidon file version 42.42.0 in "
		         << path
		         << ": can only works with version below or equal to 0.3.0";
		EXPECT_EQ(expected.str(),e.what());

	}
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
