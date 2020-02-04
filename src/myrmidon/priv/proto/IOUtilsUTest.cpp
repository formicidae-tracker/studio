#include "IOUtilsUTest.hpp"

#include "IOUtils.hpp"

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <myrmidon/UtilsUTest.hpp>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Identifier.hpp>
#include <myrmidon/priv/Ant.hpp>
#include <myrmidon/TestSetup.hpp>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

TEST_F(IOUtilsUTest,TimeIO) {
	google::protobuf::Timestamp pbt;
	google::protobuf::util::TimeUtil::FromString("2019-01-T10:12:34.567+01:00",&pbt);

	struct TestData {
		Time     T;
		int64_t  Seconds;
		int64_t  Nanos;
		uint64_t Mono;
	};

	std::vector<TestData> data
		= {
		   //An empty time
		   {Time(),0,0,0},
		   // A Timestamp
		   {Time::FromTimeT(1234),1234,0,0},
		   // A more complicated timestamp
		   {Time::FromTimestamp(pbt),pbt.seconds(),pbt.nanos(),0},
		   // A Time with a monotonic value
		   {
		    Time::FromTimestampAndMonotonic(pbt, 123456789, 42),
		    pbt.seconds(),
		    pbt.nanos(),
		    123456789
		   },
	};

	for (const auto & d : data ) {
		pb::Time t;
		pb::Time expected;
		expected.mutable_timestamp()->set_seconds(d.Seconds);
		expected.mutable_timestamp()->set_nanos(d.Nanos);
		expected.set_monotonic(d.Mono);
		IOUtils::SaveTime(&t, d.T);

		ExpectMessageEquals(t,expected);

		auto res = IOUtils::LoadTime(t, 42);

		EXPECT_TRUE(TimeEqual(res,d.T));
	}

}


TEST_F(IOUtilsUTest,IdentificationIO) {
	struct TestData {
		Time::ConstPtr Start,End;
		double         X,Y,Angle;
		TagID          Value;
	};

	std::vector<TestData> data
		= {
		   {
		    Time::ConstPtr(),Time::ConstPtr(),
		    1.0,-1.0,M_PI/2,
		    123
		   },
		   {
		    std::make_shared<Time>(Time::FromTimeT(2)),Time::ConstPtr(),
		    1.45,-4.0,3*M_PI/4,
		    23
		   },
		   {
		    Time::ConstPtr(),std::make_shared<Time>(Time::FromTimeT(2)),
		    1.0,-1.0,0.0,
		    34
		   },
	};

	auto e = Experiment::Create(TestSetup::Basedir()/ "test.myrmidon");
	auto a = e->Identifier().CreateAnt();
	for ( const auto & d : data ) {
		auto ident = e->Identifier().AddIdentification(a->ID(), d.Value, d.Start, d.End);
		ident->SetAntPosition(Eigen::Vector2d(d.X,d.Y), d.Angle);

		pb::Identification identPb;
		pb::Identification expected;
		if ( d.Start ) {
			d.Start->ToTimestamp(expected.mutable_start());
		}
		if ( d.End ) {
			d.End->ToTimestamp(expected.mutable_end());
		}
		expected.mutable_antposition()->set_x(d.X);
		expected.mutable_antposition()->set_y(d.Y);
		expected.set_antangle(d.Angle);
		expected.set_id(d.Value);

		IOUtils::SaveIdentification(&identPb, ident);
		ExpectMessageEquals(identPb,expected);

		e->Identifier().DeleteIdentification(ident);
		ASSERT_TRUE(a->Identifications().empty());
		IOUtils::LoadIdentification(*e,a,identPb);

		EXPECT_EQ(a->Identifications().size(),1);

		if ( a->Identifications().empty() ) {
			continue;
		}
		auto finalIdent = a->Identifications()[0];
		EXPECT_EQ(finalIdent->TagValue(),d.Value);
		EXPECT_EQ(!finalIdent->Start(),!d.Start);
		EXPECT_TRUE(TimePtrEqual(finalIdent->Start(),d.Start));
		EXPECT_TRUE(TimePtrEqual(finalIdent->End(),d.End));
		EXPECT_FLOAT_EQ(finalIdent->AntPosition().x(),d.X);
		EXPECT_FLOAT_EQ(finalIdent->AntPosition().y(),d.Y);
		EXPECT_FLOAT_EQ(finalIdent->AntAngle(),d.Angle);
		EXPECT_NO_THROW({
				EXPECT_EQ(finalIdent->Target().get(),a.get());
			});
		e->Identifier().DeleteIdentification(finalIdent);
	}

}

TEST_F(IOUtilsUTest,VectorIO) {
	struct TestData {
		double X,Y;
	};

	std::vector<TestData> testdata
		= {
		   {0.0,0.0},
		   {1.23,4.67},
	};

	for(const auto & d: testdata) {
		Eigen::Vector2d dV(d.X,d.Y),res;
		pb::Vector2d v,expected;
		expected.set_x(dV.x());
		expected.set_y(dV.y());

		IOUtils::SaveVector(&v,dV);
		ExpectMessageEquals(v,expected);

		IOUtils::LoadVector(res,v);
		ExpectAlmostEqualVector(res,dV);
	}
}

TEST_F(IOUtilsUTest,CapsuleIO) {
	struct TestData {
		double AX,AY,AR;
		double BX,BY,BR;
	};

	std::vector<TestData> testdata
		= {
		   {
		    0.0,0.0,1.0,
		    1.0,1.0,0.5
		   },
	};

	for(const auto & d: testdata) {
		Eigen::Vector2d dA(d.AX,d.AY),dB(d.BX,d.BY);
		auto dC = std::make_shared<Capsule>(dA,dB,d.AR,d.BR);
		pb::Capsule c,expected;
		IOUtils::SaveVector(expected.mutable_a(),dA);
		IOUtils::SaveVector(expected.mutable_b(),dB);
		expected.set_a_radius(d.AR);
		expected.set_b_radius(d.BR);

		IOUtils::SaveCapsule(&c,dC);
		ExpectMessageEquals(c,expected);

		auto res = IOUtils::LoadCapsule(c);
		ExpectAlmostEqualVector(res->A(),dC->A());
		ExpectAlmostEqualVector(res->B(),dC->B());
		EXPECT_DOUBLE_EQ(res->RadiusA(),dC->RadiusA());
		EXPECT_DOUBLE_EQ(res->RadiusB(),dC->RadiusB());
	}

}


TEST_F(IOUtilsUTest,AntIO) {
	struct IdentificationData {
		Time::ConstPtr Start,End;
		double X,Y,Angle;
		TagID Value;
	};

	struct MeasurementData {
		std::string Name;
		double Value;
	};

	struct TestData {
		std::vector<IdentificationData> IData;
		std::vector<CapsulePtr>         Capsules;
		std::vector<MeasurementData>    Measurements;
	};

	std::vector<TestData> testdata
		= {
		   {
		    {
		     {
		      Time::ConstPtr(),std::make_shared<Time>(Time::FromTimeT(1)),
		      2.0,3.0,M_PI,
		      1,
		     },
		     {
		      std::make_shared<Time>(Time::FromTimeT(2)),Time::ConstPtr(),
		      2.0,3.0,M_PI,
		      2,
		     },
		    },
		    {
		     std::make_shared<Capsule>(Eigen::Vector2d(2.0,-4.0),
		                               Eigen::Vector2d(23.1,-7.3),
		                               1.0,2.0),
		     std::make_shared<Capsule>(Eigen::Vector2d(13.0,23.0),
		                               Eigen::Vector2d(6.1,8.9),
		                               5.0,-3.0)
		    },
		    {
		     {"length", 3.6},
		     {"antennas",0.47}
		    }
		   }
	};

	auto e = Experiment::Create(TestSetup::Basedir() / "test-ant-io.myrmidon");

	for(auto & d: testdata) {
		std::sort(d.Measurements.begin(),
		          d.Measurements.end(),
		          [](const MeasurementData & a,
		             const MeasurementData & b) {
			          return a.Name < b.Name;
		          });
		auto dA = e->Identifier().CreateAnt();
		std::vector<Identification::Ptr> dIdents;

		pb::AntMetadata a,expected;
		expected.set_id(dA->ID());
		for(const auto & identData : d.IData ) {
			auto ident  =e->Identifier().AddIdentification(dA->ID(),
			                                               identData.Value,
			                                               identData.Start,
			                                               identData.End);
			ident->SetAntPosition(Eigen::Vector2d(identData.X,identData.Y),
			                      identData.Angle);
			dIdents.push_back(ident);
			IOUtils::SaveIdentification(expected.add_identifications(), ident);
		}

		for ( const auto & c : d.Capsules ) {
			dA->AddCapsule(c);
			IOUtils::SaveCapsule(expected.mutable_shape()->add_capsules(),
			                     c);
		}

		IOUtils::SaveAnt(&a,dA);
		std::string differences;

		ExpectMessageEquals(a,expected);

		EXPECT_THROW({
				IOUtils::LoadAnt(*e,a);
			},std::exception);

		EXPECT_NO_THROW({
				for( auto & i : dIdents ) {
					e->Identifier().DeleteIdentification(i);
				}
				e->Identifier().DeleteAnt(dA->ID());
			});

		IOUtils::LoadAnt(*e,a);
		auto fi = e->Identifier().Ants().find(expected.id());
		EXPECT_TRUE(fi != e->Identifier().Ants().cend());
		if ( fi == e->Identifier().Ants().cend() ) {
			continue;
		}
		auto res = fi->second;
		EXPECT_EQ(res->ID(),expected.id());
		EXPECT_EQ(res->Identifications().size(),dIdents.size());
		for(size_t i = 0 ;
		    i < std::min(res->Identifications().size(),dIdents.size());
		    ++i) {
			auto ii = res->Identifications()[i];
			auto ie = dIdents[i];
			EXPECT_EQ(ii->TagValue(),ie->TagValue());
			EXPECT_TRUE(TimePtrEqual(ii->Start(),ie->Start()));
			EXPECT_TRUE(TimePtrEqual(ii->End(),ie->End()));
			ExpectAlmostEqualVector(ii->AntPosition(),ie->AntPosition());
			EXPECT_DOUBLE_EQ(ii->AntAngle(),ie->AntAngle());
			EXPECT_EQ(ii->Target()->ID(),ie->Target()->ID());

		}

		EXPECT_EQ(res->Shape().size(),
		          d.Capsules.size());
		for(size_t i = 0;
		    i < std::min(d.Capsules.size(),res->Shape().size());
		    ++i) {
			auto c = res->Shape()[i];
			auto ce = d.Capsules[i];
			ExpectAlmostEqualVector(c->A(),ce->A());
			ExpectAlmostEqualVector(c->B(),ce->B());
			EXPECT_DOUBLE_EQ(c->RadiusA(),ce->RadiusA());
			EXPECT_DOUBLE_EQ(c->RadiusB(),ce->RadiusB());
		}

	}

}


TEST_F(IOUtilsUTest,ExperimentIO) {
	auto e = Experiment::Create(TestSetup::Basedir() / "experiment-io.myrmidon");
	auto res = Experiment::Create(TestSetup::Basedir() / "experiment-io-res.myrmidon");
	e->SetFamily(tags::Family(1234));
	pb::Experiment ePb,expected;
	EXPECT_THROW({
			IOUtils::SaveExperiment(&ePb,*e);
		},std::runtime_error);

	ePb.set_tagfamily(pb::TagFamily(1234));
	EXPECT_THROW({
			IOUtils::LoadExperiment(*e, ePb);
		},std::runtime_error);

	TrackingDataDirectory::ConstPtr tdd;

	EXPECT_NO_THROW({
			e->SetAuthor("Someone");
			expected.set_author("Someone");
			e->SetName("Some experiment");
			expected.set_name("Some experiment");
			e->SetComment("Some comment");
			expected.set_comment("Some comment");
			e->SetFamily(tags::Family::Tag36h11);
			expected.set_tagfamily(pb::TAG36H11);
			e->SetThreshold(45);
			expected.set_threshold(45);
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
			e->AddTrackingDataDirectory(tdd);
			expected.add_trackingdatadirectories(tdd->URI().generic_string());
		});


	ePb.Clear();

	IOUtils::SaveExperiment(&ePb,*e);
	ExpectMessageEquals(ePb,expected);

	IOUtils::LoadExperiment(*res,ePb);
	EXPECT_EQ(res->Author(),e->Author());
	EXPECT_EQ(res->Name(),e->Name());
	EXPECT_EQ(res->Comment(),e->Comment());
	EXPECT_EQ(res->Family(),e->Family());
	EXPECT_EQ(res->Threshold(),e->Threshold());
	EXPECT_EQ(res->TrackingDataDirectories().size(),
	          e->TrackingDataDirectories().size());
	for ( const auto & [URI,tdd] : e->TrackingDataDirectories() ) {
		auto fi = res->TrackingDataDirectories().find(URI);
		if ( fi == res->TrackingDataDirectories().cend() ) {
			ADD_FAILURE() << "Could not find TrackingDataDirectory:" << URI;
			continue;
		}
		EXPECT_EQ(fi->second->URI(),tdd->URI());
		EXPECT_EQ(fi->second->AbsoluteFilePath(),tdd->AbsoluteFilePath());

	}

}

TEST_F(IOUtilsUTest,TrackingIndexIO) {
	fs::path parentURI("foo");
	Time::MonoclockID monoID(42);
	auto si = std::make_shared<TrackingDataDirectory::TrackingIndex>();
	auto res = std::make_shared<TrackingDataDirectory::TrackingIndex>();
	google::protobuf::RepeatedPtrField<pb::TrackingSegment> expected,pbRes;
	for(size_t i = 0; i < 20; ++i) {
		uint64_t fid = 100 * i;
		Time t = Time::FromTimestampAndMonotonic(Time::FromTimeT(i).ToTimestamp(),
		                                         1000000001ULL*uint64_t(i) + 10,
		                                         monoID);
		std::ostringstream os;
		os << i;
		FrameReference ref(parentURI,fid,t);
		si->Insert(ref,os.str());

		auto pb = expected.Add();
		IOUtils::SaveFrameReference(pb->mutable_frame(),ref);
		pb->set_filename(os.str());
	}
	ASSERT_EQ(si->Segments().size(),20);

	for( const auto & s : si->Segments() ) {
		auto pb = pbRes.Add();
		IOUtils::SaveTrackingIndexSegment(pb, s);
	}

	ASSERT_EQ(pbRes.size(),expected.size());
	for(size_t i = 0; i < pbRes.size(); ++i) {
		ExpectMessageEquals(pbRes.Get(i),expected.Get(i));
	}
	for (const auto & pb : pbRes ) {
		auto s = IOUtils::LoadTrackingIndexSegment(pb, parentURI,monoID);
		res->Insert(s.first,s.second);
	}
	auto ress = res->Segments();
	auto expecteds = si->Segments();

	ASSERT_EQ(ress.size(),expecteds.size());
	for(size_t i = 0; i < ress.size(); ++i) {
		auto & iref = ress[i].first;
		auto & ifilename = ress[i].second;

		EXPECT_EQ(iref.ParentURI(),parentURI);
		EXPECT_TRUE(TimeEqual(iref.Time(),expecteds[i].first.Time()));
		EXPECT_EQ(ifilename,expecteds[i].second);
	}

}


TEST_F(IOUtilsUTest,MovieSegmentIO) {
	MovieSegment::Ptr ms,res;
	Time::MonoclockID monoID(42);
	Time startTime = Time::FromTimestampAndMonotonic(Time::FromTimeT(1).ToTimestamp(),
	                                                 123456789,
	                                                 monoID);


	fort::myrmidon::pb::MovieSegment expected,pbRes;
	MovieSegment::ListOfOffset offsets;
	offsets.push_back(std::make_pair(0,1234));
	offsets.push_back(std::make_pair(42,1236));
	offsets.push_back(std::make_pair(12,1235));

	std::sort(offsets.begin(),offsets.end());
	std::reverse(offsets.begin(),offsets.end());

	ms = std::make_shared<MovieSegment>(0,
	                                    TestSetup::Basedir() / "foo.0000" / "stream.0000.mp4",
	                                    "foo.0000",
	                                    1234,
	                                    1234+100+2,
	                                    0,
	                                    100,
	                                    offsets);

	expected.set_path("stream.0000.mp4");
	expected.set_trackingstart(1234);
	expected.set_trackingend(1234+100+2);
	expected.set_moviestart(0);
	expected.set_movieend(100);
	for ( const auto & o : offsets ) {
		auto pbo = expected.add_offsets();
		pbo->set_movieframeid(o.first);
		pbo->set_offset(o.second);
	}

	IOUtils::SaveMovieSegment(&pbRes,ms,TestSetup::Basedir() / "foo.0000");
	ExpectMessageEquals(pbRes,expected);

	res = IOUtils::LoadMovieSegment(pbRes, TestSetup::Basedir() / "foo.0000" , "foo.0000");

	EXPECT_EQ(res->AbsoluteFilePath().string(), ms->AbsoluteFilePath().string());
	EXPECT_EQ(res->StartFrame(),ms->StartFrame());
	EXPECT_EQ(res->EndFrame(),ms->EndFrame());

	EXPECT_EQ(res->StartMovieFrame(),ms->StartMovieFrame());
	EXPECT_EQ(res->EndMovieFrame(),ms->EndMovieFrame());

	ASSERT_EQ(res->Offsets().size(),ms->Offsets().size());

	for (size_t i = 0; i < ms->Offsets().size(); ++i) {
		EXPECT_EQ(res->Offsets()[i],ms->Offsets()[i]);
	}

	//not using an absolute path as arguments
	EXPECT_THROW({
			IOUtils::SaveMovieSegment(&pbRes,ms, "foo.0000");
		},std::invalid_argument);
	EXPECT_THROW({
			IOUtils::LoadMovieSegment(pbRes, "foo.0000","foo.0000");
		},std::invalid_argument);
}


// TEST_F(ProtobufExperimentReadWriterUTest,TrackingDataDirectoryIO) {
// 	fort::myrmidon::pb::TrackingDataDirectory pbTdd,encoded;
// 	//pbTdd.set_path("foo.0001");
// 	pbTdd.set_startframe(5);
// 	pbTdd.set_endframe(8);
// 	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",pbTdd.mutable_startdate()->mutable_timestamp()));
// 	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",pbTdd.mutable_enddate()->mutable_timestamp()));

// 	auto tdd = ProtobufReadWriter::LoadTrackingDataDirectory(pbTdd,"foo");

// 	ASSERT_EQ(tdd.URI(),"foo.0001");
// 	ASSERT_EQ(tdd.StartFrame(),5);
// 	ASSERT_EQ(tdd.EndFrame(),8);
// 	ASSERT_EQ(tdd.StartDate().ToTimestamp(),pbTdd.startdate().timestamp());
// 	ASSERT_EQ(tdd.EndDate().ToTimestamp(),pbTdd.enddate().timestamp());

// 	EXPECT_NO_THROW({ProtobufReadWriter::SaveTrackingDataDirectory(encoded,tdd);});
// 	ASSERT_EQ(google::protobuf::util::MessageDifferencer::Equals(encoded,pbTdd),true);

// }


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
