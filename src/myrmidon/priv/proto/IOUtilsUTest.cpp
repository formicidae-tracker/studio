#include "IOUtilsUTest.hpp"

#include "IOUtils.hpp"

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <myrmidon/UtilsUTest.hpp>

#include <myrmidon/priv/Experiment.hpp>
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
		EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(t,expected));
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
		    2.0,-1.0,M_PI/2,
		    123
		   },
		   {
		    std::make_shared<Time>(Time::FromTimeT(2)),Time::ConstPtr(),
		    0.0,-4.0,3*M_PI/4,
		    23
		   },
		   {
		    Time::ConstPtr(),std::make_shared<Time>(Time::FromTimeT(2)),
		    10.0,-1.0,0.0,
		    34
		   },
	};

	auto e = Experiment::Create(TestSetup::Basedir()/ "test.myrmidon");
	auto a = e->Identifier().CreateAnt();
	for ( const auto & d : data ) {
		auto ident = e->Identifier().AddIdentification(a->ID(), d.Value, d.Start, d.End);
		ident->SetTagPosition(Eigen::Vector2d(d.X,d.Y), d.Angle);

		pb::Identification identPb;
		pb::Identification expected;
		if ( d.Start ) {
			d.Start->ToTimestamp(expected.mutable_start());
		}
		if ( d.End ) {
			d.End->ToTimestamp(expected.mutable_end());
		}
		expected.set_x(d.X);
		expected.set_y(d.Y);
		expected.set_theta(d.Angle);
		expected.set_id(d.Value);

		IOUtils::SaveIdentification(&identPb, ident);
		EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(identPb,expected))
			<< "Messages are different saved: "
			<< identPb.DebugString() << std::endl
			<< "expected: "
			<< expected.DebugString();

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
		EXPECT_FLOAT_EQ(finalIdent->TagPosition().x(),d.X);
		EXPECT_FLOAT_EQ(finalIdent->TagPosition().y(),d.Y);
		EXPECT_FLOAT_EQ(finalIdent->TagAngle(),d.Angle);
		EXPECT_NO_THROW({
				EXPECT_EQ(finalIdent->Target().get(),a.get());
			});
		e->Identifier().DeleteIdentification(finalIdent);
	}




}

TEST_F(IOUtilsUTest,AntsIO) {


}


// TEST_F(ProtobufExperimentReadWriterUTest,SegmentIndexerIO) {
// 	SegmentIndexer<std::string> si,res;
// 	google::protobuf::RepeatedPtrField<pb::TrackingSegment> expected,pbRes;
// 	for(size_t i = 0; i < 20; ++i) {
// 		uint64_t fid = 100 * i;
// 		Time t = Time::FromTimeT(100*i);
// 		std::ostringstream os;
// 		os << i;
// 		//		si.Insert(fid,t,os.str());

// 		auto pb = expected.Add();
// 		pb->set_frameid(fid);
// 		t.ToTimestamp(*pb->mutable_time()->mutable_timestamp());
// 		pb->set_path(os.str());
// 	}

// 	ProtobufReadWriter::SaveSegmentIndexer(&pbRes, si);
// 	ASSERT_EQ(pbRes.size(),expected.size());
// 	for(size_t i = 0; i < pbRes.size(); ++i) {
// 		EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(pbRes.Get(i),expected.Get(i)));
// 	}
// 	ProtobufReadWriter::LoadSegmentIndexer(res, pbRes, 42);
// 	auto ress = res.Segments();
// 	auto expecteds = si.Segments();

// 	ASSERT_EQ(ress.size(),expecteds.size());
// 	for(size_t i = 0; i < ress.size(); ++i) {
// 		EXPECT_EQ(std::get<0>(ress[i]),std::get<0>(expecteds[i]));
// 		EXPECT_TRUE(TimeEqual(std::get<1>(ress[i]),std::get<1>(expecteds[i])));
// 		EXPECT_EQ(std::get<2>(ress[i]),std::get<2>(expecteds[i]));
// 	}

// }


// TEST_F(ProtobufExperimentReadWriterUTest,MovieSegmentIO) {
// 	MovieSegment::Ptr ms,res;
// 	fort::myrmidon::pb::MovieSegment expected,pbRes;
// 	MovieSegment::ListOfOffset offsets;
// 	offsets.push_back(std::make_pair(0,1234));
// 	offsets.push_back(std::make_pair(42,1236));
// 	offsets.push_back(std::make_pair(12,1235));

// 	std::sort(offsets.begin(),offsets.end());
// 	std::reverse(offsets.begin(),offsets.end());

// 	ms = std::make_shared<MovieSegment>("bar/foo",1234,1234+100+2,0,100,offsets);

// 	expected.set_path("foo");
// 	expected.set_trackingstart(1234);
// 	expected.set_trackingend(1234+100+2);
// 	expected.set_moviestart(0);
// 	expected.set_movieend(100);
// 	for ( const auto & o : offsets ) {
// 		auto pbo = expected.add_offsets();
// 		pbo->set_movieframeid(o.first);
// 		pbo->set_offset(o.second);
// 	}



// 	ProtobufReadWriter::SaveMovieSegment(&pbRes,ms,"bar");
// 	EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(pbRes,expected)) <<
// 		"Got " << pbRes.ShortDebugString() << " but expected: " << expected.ShortDebugString();

// 	res = ProtobufReadWriter::LoadMovieSegment(pbRes, "bar");
// 	EXPECT_EQ(res->MovieFilepath(), ms->MovieFilepath());
// 	EXPECT_EQ(res->StartFrame(),ms->StartFrame());
// 	EXPECT_EQ(res->EndFrame(),ms->EndFrame());

// 	EXPECT_EQ(res->StartMovieFrame(),ms->StartMovieFrame());
// 	EXPECT_EQ(res->EndMovieFrame(),ms->EndMovieFrame());

// 	ASSERT_EQ(res->Offsets().size(),ms->Offsets().size());

// 	for (size_t i = 0; i < ms->Offsets().size(); ++i) {
// 		EXPECT_EQ(res->Offsets()[i],ms->Offsets()[i]);
// 	}


// }


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
