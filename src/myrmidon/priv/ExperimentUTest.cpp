#include "ExperimentUTest.hpp"

#include "Experiment.hpp"
#include "Ant.hpp"
#include <myrmidon/TestSetup.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <myrmidon/priv/Identifier.hpp>
#include <myrmidon/priv/Space.hpp>
#include <myrmidon/priv/Measurement.hpp>
#include <myrmidon/UtilsUTest.hpp>
#include <fstream>

namespace fort {
namespace myrmidon {
namespace priv {

void ReadAll(const fs::path & a, std::vector<uint8_t> & data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(),std::ios::binary);
	data =  std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});
}


TEST_F(ExperimentUTest,CanAddTrackingDataDirectory) {
	try {
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon");
		auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002", TestSetup::Basedir());
		ASSERT_FALSE(e->Spaces().empty());
		e->Spaces()[0]->AddTrackingDataDirectory(tdd);

		ASSERT_EQ(e->Spaces()[0]->TrackingDataDirectories().size(),2);
		e->Save(TestSetup::Basedir() / "test3.myrmidon");
		auto ee = Experiment::Open(TestSetup::Basedir() / "test3.myrmidon");

		ASSERT_FALSE(ee->Spaces().empty());
		ASSERT_EQ(ee->Spaces()[0]->TrackingDataDirectories().size(),2);


	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentUTest,IOTest) {
	try{
		auto e = Experiment::Open(TestSetup::Basedir() / "test.myrmidon" );
		ASSERT_FALSE(e->Spaces().empty());
		auto tdd = e->Spaces()[0]->TrackingDataDirectories();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd[0]->URI(),"foo.0000");
		ASSERT_EQ(tdd[0]->AbsoluteFilePath(),TestSetup::Basedir() / "foo.0000");
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
	ExperimentPtr e;
	TrackingDataDirectory::ConstPtr foo0,foo1;
	Space::Ptr z;
	ASSERT_NO_THROW({
			e = Experiment::NewFile(TestSetup::Basedir() / "new-file.myrmidon");
			foo0 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
			foo1 = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir());
			z = e->CreateSpace("box");
			z->AddTrackingDataDirectory(foo0);
			z->AddTrackingDataDirectory(foo1);
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
			e->CreateMeasurementType(Measurement::HEAD_TAIL_TYPE,
			                         "foo");
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

	EXPECT_EQ(e->NextAvailableMeasurementTypeID(),
	          Measurement::HEAD_TAIL_TYPE+1);

	EXPECT_NO_THROW({
			e->CreateMeasurementType(Measurement::HEAD_TAIL_TYPE+2,
			                         "foo");
		});

	EXPECT_EQ(e->NextAvailableMeasurementTypeID(),
	          Measurement::HEAD_TAIL_TYPE+1);

	EXPECT_NO_THROW({
			// its ok to be clumsy and use the same names for different type
			e->CreateMeasurementType(Measurement::HEAD_TAIL_TYPE+1,
			                         "foo");
		});

	EXPECT_EQ(e->NextAvailableMeasurementTypeID(),
	          Measurement::HEAD_TAIL_TYPE+3);

	auto tcuPath = foo0->URI() / "frames" / std::to_string(foo0->StartFrame()) / "closeups/21";
	auto badPath = fs::path("bar.0000") / "frames" / std::to_string(foo0->StartFrame()) / "closeups/21";

	auto goodCustom = std::make_shared<Measurement>(tcuPath,
	                                                Measurement::HEAD_TAIL_TYPE+1,
	                                                Eigen::Vector2d(12,1),
	                                                Eigen::Vector2d(1,12),
	                                                12.0);
	auto goodDefault = std::make_shared<Measurement>(tcuPath,
	                                                 Measurement::HEAD_TAIL_TYPE,
	                                                 Eigen::Vector2d(12,12),
	                                                 Eigen::Vector2d(10,12),
	                                                 12.0);
	auto defaultWithBadPath = std::make_shared<Measurement>(badPath,
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

	auto antBefore = e->Identifier().CreateAnt(0);
	auto identBefore1 = e->Identifier().AddIdentification(antBefore->ID(),
	                                                      1,
	                                                      Time::ConstPtr(),
	                                                      std::make_shared<Time>(foo0->EndDate()));
	identBefore1->SetTagSize(2.0);

	auto identBefore2 = e->Identifier().AddIdentification(antBefore->ID(),
	                                                      0,
	                                                      std::make_shared<Time>(foo1->StartDate()),
	                                                      Time::ConstPtr());
	identBefore2->SetTagSize(2.0);


	struct MData {
		TrackingDataDirectory::ConstPtr TDD;
		size_t                          Offset;
		TagID                           TID;
		MeasurementType::ID             MTID;
	};
	std::vector<MData> mData =
		{
		 {foo0,0,0,0},
		 {foo0,0,0,1},
		 {foo0,0,1,0},
		 {foo0,0,1,1},
		 {foo0,1,0,0},
		 {foo0,1,0,1},
		 {foo0,1,1,0},
		 {foo0,1,1,1},
		 {foo1,0,0,0},
		 {foo1,0,0,1},
		 {foo1,0,1,0},
		 {foo1,0,1,1},
		 {foo1,1,0,0},
		 {foo1,1,0,1},
		 {foo1,1,1,0},
		 {foo1,1,1,1}
		};
	std::vector<fs::path> paths;
	paths.reserve(mData.size());



	for ( const auto & md : mData ) {
		auto tcuPath = md.TDD->URI()
			/ "frames"
			/ std::to_string(md.TDD->StartFrame() + md.Offset)
			/ "closeups"
			/ std::to_string(md.TID);

		auto m = std::make_shared<Measurement>(tcuPath,
		                                       md.MTID,
		                                       Eigen::Vector2d(12,0),
		                                       Eigen::Vector2d(0,0),
		                                       1.0);
		paths.push_back(m->URI());
		e->SetMeasurement(m);
	}

	//Now we add a super Ant
	auto antAfter = e->Identifier().CreateAnt(0);
	auto identAfter1 = e->Identifier().AddIdentification(antAfter->ID(),
	                                                     0,
	                                                     Time::ConstPtr(),
	                                                     std::make_shared<Time>(foo0->EndDate()));

	auto identAfter2 = e->Identifier().AddIdentification(antAfter->ID(),
	                                                     1,
	                                                     std::make_shared<Time>(foo1->StartDate()),
	                                                     Time::ConstPtr());
	e->SetFamily(tags::Family::Tag36ARTag);
	e->SetDefaultTagSize(1.0);
	EXPECT_TRUE(VectorAlmostEqual(identAfter1->AntPosition(),
	                              Eigen::Vector2d(6.0,0.0)));

	std::vector<Experiment::ComputedMeasurement> measurements;
	e->ComputeMeasurementsForAnt(measurements,
	                             antAfter->ID(),
	                             0);

	EXPECT_EQ(measurements.size(), 4);
	for(const auto & m : measurements) {
		EXPECT_EQ(12.0,m.LengthMM);
	}

	EXPECT_TRUE(VectorAlmostEqual(identBefore1->AntPosition(),
	                              Eigen::Vector2d(6.0,0.0)));


	e->ComputeMeasurementsForAnt(measurements,
	                             antBefore->ID(),
	                             0);

	EXPECT_EQ(measurements.size(), 4);
	for(const auto & m : measurements) {
		EXPECT_EQ(24.0,m.LengthMM);
	}

	EXPECT_THROW({
			e->ComputeMeasurementsForAnt(measurements,
			                             antAfter->ID() + 100,
			                             0);
		},Identifier::UnmanagedAnt);


	auto antLast = e->Identifier().CreateAnt();
	e->Identifier().AddIdentification(antLast->ID(),
	                                  22,
	                                  Time::ConstPtr(),
	                                  Time::ConstPtr());

	e->ComputeMeasurementsForAnt(measurements,
	                             antAfter->ID(),
	                             4);
	EXPECT_EQ(measurements.size(),0);

	e->ComputeMeasurementsForAnt(measurements,
	                             antLast->ID(),
	                             0);
	EXPECT_EQ(measurements.size(),0);


	for ( const auto & uri : paths ) {
		e->DeleteMeasurement(uri);
	}

	EXPECT_THROW({
			e->DeleteMeasurement("none/frames/23/closeups/43/measurements/0");
		},std::invalid_argument);

	EXPECT_THROW({
			e->DeleteMeasurement("foo.0000/frames/0/closeups/43/measurements/0");
		},std::runtime_error);

	EXPECT_THROW({
			e->DeleteMeasurement("foo.0000/frames/0/closeups/21/measurements/34");
		},std::runtime_error);

	EXPECT_NO_THROW({
			//OK it has no measurement
			e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE+2);
		});

	EXPECT_THROW({
			// contains a tracking data directory
			e->DeleteSpace(z->URI());
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
			e->DeleteSpace(z->URI());
		});



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


} //namespace priv
} //namespace myrmidon
} //namespace fort
