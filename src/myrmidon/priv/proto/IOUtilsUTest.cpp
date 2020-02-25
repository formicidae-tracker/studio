#include "IOUtilsUTest.hpp"

#include "IOUtils.hpp"

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <myrmidon/UtilsUTest.hpp>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Identifier.hpp>
#include <myrmidon/priv/Ant.hpp>
#include <myrmidon/priv/Measurement.hpp>
#include <myrmidon/priv/Space.hpp>
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

		EXPECT_TRUE(MessageEqual(t,expected));

		auto res = IOUtils::LoadTime(t, 42);

		EXPECT_TRUE(TimeEqual(res,d.T));
	}

}


TEST_F(IOUtilsUTest,IdentificationIO) {
	struct TestData {
		Time::ConstPtr Start,End;
		double         TagSize;
		TagID          Value;
	};

	std::vector<TestData> data
		= {
		   {
		    Time::ConstPtr(),Time::ConstPtr(),
		    0.0,
		    123
		   },
		   {
		    std::make_shared<Time>(Time::FromTimeT(2)),Time::ConstPtr(),
		    2.3,
		    23
		   },
		   {
		    Time::ConstPtr(),std::make_shared<Time>(Time::FromTimeT(2)),
		    0.0,
		    34
		   },
	};

	auto e = Experiment::Create(TestSetup::Basedir()/ "test.myrmidon");
	auto a = e->Identifier().CreateAnt();
	for ( const auto & d : data ) {
		auto ident = e->Identifier().AddIdentification(a->ID(), d.Value, d.Start, d.End);
		// ident->SetAntPosition(Eigen::Vector2d(d.X,d.Y), d.Angle);
		ident->SetTagSize(d.TagSize);
		pb::Identification identPb;
		pb::Identification expected;
		if ( d.Start ) {
			d.Start->ToTimestamp(expected.mutable_start());
		}
		if ( d.End ) {
			d.End->ToTimestamp(expected.mutable_end());
		}
		expected.set_id(d.Value);
		expected.set_tagsize(d.TagSize);

		IOUtils::SaveIdentification(&identPb, ident);
		EXPECT_TRUE(MessageEqual(identPb,expected));

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
		EXPECT_FLOAT_EQ(finalIdent->AntPosition().x(),0);
		EXPECT_FLOAT_EQ(finalIdent->AntPosition().y(),0);
		EXPECT_FLOAT_EQ(finalIdent->AntAngle(),0);
		if ( d.TagSize == 0.0 ) {
			EXPECT_TRUE(finalIdent->UseDefaultTagSize());
		} else {
			EXPECT_DOUBLE_EQ(finalIdent->TagSize(),d.TagSize);
		}
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
		EXPECT_TRUE(MessageEqual(v,expected));

		IOUtils::LoadVector(res,v);
		EXPECT_TRUE(VectorAlmostEqual(res,dV));
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
		EXPECT_TRUE(MessageEqual(c,expected));

		auto res = IOUtils::LoadCapsule(c);
		EXPECT_TRUE(VectorAlmostEqual(res->C1(),dC->C1()));
		EXPECT_TRUE(VectorAlmostEqual(res->C2(),dC->C2()));
		EXPECT_DOUBLE_EQ(res->R1(),dC->R1());
		EXPECT_DOUBLE_EQ(res->R2(),dC->R2());
	}

}


TEST_F(IOUtilsUTest,AntIO) {
	struct IdentificationData {
		Time::ConstPtr    Start,End;
		double            X,Y,Angle;
		TagID             Value;
	};


	struct TestData {
		std::vector<IdentificationData> IData;
		std::vector<CapsulePtr>         Capsules;
		Color                           DisplayColor;
		Ant::DisplayState               DisplayState;
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
		    {127,56,94},
		    Ant::DisplayState::SOLO,
		   },
	};

	auto e = Experiment::Create(TestSetup::Basedir() / "test-ant-io.myrmidon");

	for(auto & d: testdata) {
		auto dA = e->Identifier().CreateAnt();
		std::vector<Identification::Ptr> dIdents;

		pb::AntMetadata a,expected;
		expected.set_id(dA->ID());
		for(const auto & identData : d.IData ) {
			auto ident  =e->Identifier().AddIdentification(dA->ID(),
			                                               identData.Value,
			                                               identData.Start,
			                                               identData.End);
			dIdents.push_back(ident);
			IOUtils::SaveIdentification(expected.add_identifications(), ident);
		}

		for ( const auto & c : d.Capsules ) {
			dA->AddCapsule(c);
			IOUtils::SaveCapsule(expected.mutable_shape()->add_capsules(),
			                     c);
		}

		dA->SetDisplayColor(d.DisplayColor);
		IOUtils::SaveColor(expected.mutable_color(),d.DisplayColor);
		dA->SetDisplayStatus(d.DisplayState);
		expected.set_displaystate(IOUtils::SaveAntDisplayState(d.DisplayState));

		IOUtils::SaveAnt(&a,dA);
		std::string differences;

		EXPECT_TRUE(MessageEqual(a,expected));

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
			EXPECT_TRUE(VectorAlmostEqual(ii->AntPosition(),ie->AntPosition()));
			EXPECT_NEAR(ii->AntAngle(),ie->AntAngle(),M_PI/100000.0);
			EXPECT_EQ(ii->Target()->ID(),ie->Target()->ID());

		}

		EXPECT_EQ(res->Shape().size(),
		          d.Capsules.size());
		for(size_t i = 0;
		    i < std::min(d.Capsules.size(),res->Shape().size());
		    ++i) {
			auto c = res->Shape()[i];
			auto ce = d.Capsules[i];
			EXPECT_TRUE(VectorAlmostEqual(c->C1(),ce->C1()));
			EXPECT_TRUE(VectorAlmostEqual(c->C2(),ce->C2()));
			EXPECT_DOUBLE_EQ(c->R1(),ce->R1());
			EXPECT_DOUBLE_EQ(c->R2(),ce->R2());
		}

		EXPECT_EQ(res->DisplayColor(),
		          d.DisplayColor);

		EXPECT_EQ(res->DisplayStatus(),
		          dA->DisplayStatus());
	}

}

TEST_F(IOUtilsUTest,MeasurementIO) {
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d       Start,End;
		std::string           ParentURI;
		MeasurementType::ID   TID;
		double                TagSizePx;
	};

	std::vector<TestData> testdata
		= {
		   {
		    Eigen::Vector2d(12.356,-23.0),Eigen::Vector2d(42.8,0.00024),
		    "foo/bar/frames/1234/closeups/342",
		    1,
		    34.256
		   },
	};

	for(const auto & d: testdata) {
		auto dM = std::make_shared<Measurement>(d.ParentURI,
		                                        d.TID,
		                                        d.Start,
		                                        d.End,
		                                        d.TagSizePx);
		pb::Measurement expected,pbRes;
		expected.set_tagcloseupuri(d.ParentURI);
		expected.set_type(d.TID);
		IOUtils::SaveVector(expected.mutable_start(),d.Start);
		IOUtils::SaveVector(expected.mutable_end(),d.End);
		expected.set_tagsizepx(d.TagSizePx);

		IOUtils::SaveMeasurement(&pbRes,dM);
		EXPECT_TRUE(MessageEqual(pbRes,expected));

		auto res = IOUtils::LoadMeasurement(pbRes);
		EXPECT_EQ(res->URI(),
		          dM->URI());

		EXPECT_EQ(res->TagCloseUpURI(),
		          dM->TagCloseUpURI());

		EXPECT_EQ(res->Type(),dM->Type());
		EXPECT_TRUE(VectorAlmostEqual(res->StartFromTag(),dM->StartFromTag()));
		EXPECT_TRUE(VectorAlmostEqual(res->EndFromTag(),dM->EndFromTag()));
		EXPECT_DOUBLE_EQ(res->TagSizePx(),dM->TagSizePx());

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
			e->SetDefaultTagSize(1.6);
			expected.set_tagsize(1.6);
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
			auto s = e->CreateSpace(0,"box");
			s->AddTrackingDataDirectory(tdd);
			auto sPb = expected.add_spaces();
			sPb->set_id(s->SpaceID());
			sPb->set_name("box");
			sPb->add_trackingdatadirectories(tdd->URI());

			e->MeasurementTypes().find(0)->second->SetName("my-head-tail");
			auto mt = expected.add_custommeasurementtypes();
			mt->set_id(0);
			mt->set_name("my-head-tail");

			e->CreateMeasurementType(1,"antennas");
			mt = expected.add_custommeasurementtypes();
			mt->set_id(1);
			mt->set_name("antennas");

		});


	ePb.Clear();

	IOUtils::SaveExperiment(&ePb,*e);
	EXPECT_TRUE(MessageEqual(ePb,expected));

	IOUtils::LoadExperiment(*res,ePb);
	EXPECT_EQ(res->Author(),e->Author());
	EXPECT_EQ(res->Name(),e->Name());
	EXPECT_EQ(res->Comment(),e->Comment());
	EXPECT_EQ(res->Family(),e->Family());
	EXPECT_EQ(res->Threshold(),e->Threshold());
	ASSERT_EQ(res->Spaces().size(),
	          e->Spaces().size());
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
	std::string parentURI("foo");
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
		EXPECT_TRUE(MessageEqual(pbRes.Get(i),expected.Get(i)));
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
	EXPECT_TRUE(MessageEqual(pbRes,expected));

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

TEST_F(IOUtilsUTest,FamilyIO) {
	struct TestData {
		tags::Family  Family;
		pb::TagFamily Pb;
	};

	std::vector<TestData> testdata =
		{
		 {tags::Family::Tag36h11,pb::TAG36H11},
		 {tags::Family::Tag36h10,pb::TAG36H10},
		 {tags::Family::Tag36ARTag,pb::TAG36ARTAG},
		 {tags::Family::Tag16h5,pb::TAG16H5},
		 {tags::Family::Tag25h9,pb::TAG25H9},
		 {tags::Family::Circle21h7,pb::CIRCLE21H7},
		 {tags::Family::Circle49h12,pb::CIRCLE49H12},
		 {tags::Family::Custom48h12,pb::CUSTOM48H12},
		 {tags::Family::Standard41h12,pb::STANDARD41H12},
		 {tags::Family::Standard52h13,pb::STANDARD52H13},
		};

	for( const auto & d: testdata ) {
		EXPECT_EQ(d.Family,IOUtils::LoadFamily(d.Pb));
		EXPECT_EQ(d.Pb,IOUtils::SaveFamily(d.Family));
	}
}

TEST_F(IOUtilsUTest,TagCloseUpIO) {
	auto basedir = TestSetup::Basedir() / "foo.0000" / "ants";
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d Position;
		fs::path        Filepath;
		FrameReference  Reference;
		TagID           TID;
		double          Angle;
		Vector2dList    Corners;
	};

	std::vector<TestData> testdata =
		{
		 {
		  Eigen::Vector2d(23.0,-3.0),
		  "ant_123_frame_21.png",
		  FrameReference("foo.0000",21,Time::FromTimeT(2)),
		  123,
		  -M_PI/ 5.0,
		  {
		   Eigen::Vector2d(43,17.0),
		   Eigen::Vector2d(43,-23.0),
		   Eigen::Vector2d(3,-23.0),
		   Eigen::Vector2d(3,17.0),
		  },
		 }
		};


	for ( const auto & d : testdata ) {
		auto dTCU = std::make_shared<TagCloseUp>(basedir / d.Filepath,
		                                         d.Reference,
		                                         d.TID,
		                                         d.Position,
		                                         d.Angle,
		                                         d.Corners);

		auto resolver = [&d](FrameID FID) {
			                return FrameReference(d.Reference.ParentURI(),
			                                      FID,
			                                      d.Reference.Time().Add( (int64_t(FID) - int64_t(d.Reference.FID())) * Duration::Second));
		                };



		pb::TagCloseUp expected,pbRes;
		IOUtils::SaveVector(expected.mutable_position(),d.Position);
		expected.set_angle(d.Angle);
		expected.set_value(d.TID);
		for (const auto & c : d.Corners ) {
			IOUtils::SaveVector(expected.add_corners(),c);
		}
		expected.set_frameid(d.Reference.FID());
		expected.set_imagepath(d.Filepath.generic_string());

		IOUtils::SaveTagCloseUp(&pbRes,dTCU,basedir);
		EXPECT_TRUE(MessageEqual(pbRes,expected));
		auto res = IOUtils::LoadTagCloseUp(pbRes,basedir,resolver);

		EXPECT_EQ(res->Frame().URI(),dTCU->Frame().URI());
		EXPECT_TRUE(TimeEqual(res->Frame().Time(),dTCU->Frame().Time()));
		EXPECT_EQ(res->URI(),dTCU->URI());
		EXPECT_EQ(res->AbsoluteFilePath(),dTCU->AbsoluteFilePath());
		EXPECT_TRUE(VectorAlmostEqual(res->TagPosition(),dTCU->TagPosition()));
		EXPECT_DOUBLE_EQ(res->TagAngle(),dTCU->TagAngle());
		ASSERT_EQ(4,res->Corners().size());
		for(size_t i = 0; i < 4 ; ++i ) {
			EXPECT_TRUE(VectorAlmostEqual(res->Corners()[i],dTCU->Corners()[i]));
		}

		EXPECT_THROW({
				// Needs 4 corners
				expected.clear_corners();
				IOUtils::LoadTagCloseUp(expected,basedir,resolver);
			},std::invalid_argument);
	}

}

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
