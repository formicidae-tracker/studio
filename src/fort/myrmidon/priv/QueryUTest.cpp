#include "QueryUTest.hpp"

#include "Query.hpp"
#include "Ant.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/Shapes.hpp>

#include <fort/myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

void QueryUTest::SetUp() {
	ASSERT_NO_THROW({
			experiment = Experiment::Create(TestSetup::Basedir() / "query.myrmidon");
			auto space = experiment->CreateSpace("box");
			experiment->AddTrackingDataDirectory(space,TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir()));
			experiment->AddTrackingDataDirectory(space,TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir()));
			experiment->AddTrackingDataDirectory(space,TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",TestSetup::Basedir()));
		});

}

TEST_F(QueryUTest,TagStatistics) {
	TagStatistics::ByTagID tagStats;
	ASSERT_NO_THROW({
			Query::ComputeTagStatistics(experiment,
			                            tagStats);
		});

	EXPECT_EQ(tagStats.size(),2);
	ASSERT_EQ(tagStats.count(123),1);
	ASSERT_EQ(tagStats.count(124),1);
	EXPECT_TRUE(TimeEqual(tagStats.at(123).FirstSeen,
	                      Time::Parse("2019-11-02T09:00:20.021Z")));

	EXPECT_TRUE(TimeEqual(tagStats.at(123).LastSeen,
	                      Time::Parse("2019-11-02T09:01:46.436070Z")));

	EXPECT_EQ(tagStats.at(123).ID,123);

	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::TOTAL_SEEN),600);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::MULTIPLE_SEEN),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_500MS),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1S),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10S),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1M),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10M),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_1H),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_10H),0);
	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::GAP_MORE),0);
}


TEST_F(QueryUTest,IdentifiedFrame) {
	ASSERT_NO_THROW({
			experiment->CreateAnt(1);
			Identifier::AddIdentification(experiment->Identifier(),1,123,Time::SinceEver(),Time::Forever());
		});

	std::vector<IdentifiedFrame::Ptr> identifieds;
	ASSERT_NO_THROW({
			myrmidon::Query::IdentifyFramesArgs args;
			Query::IdentifyFrames(experiment,
			                      [&identifieds] (const IdentifiedFrame::Ptr & i) {
				                      identifieds.push_back(i);
			                      },args);
		});
	ASSERT_EQ(identifieds.size(),600);
	for ( const auto & frame : identifieds ) {
		EXPECT_EQ(frame->Space,1);
		ASSERT_EQ(frame->Positions.size(),5);
		ASSERT_EQ(AntID(frame->Positions(0,0)),1);
	}

	auto t = experiment->Spaces().begin()->second->TrackingDataDirectories().front()->StartDate();
	identifieds.clear();
	ASSERT_NO_THROW({
			myrmidon::Query::IdentifyFramesArgs args;
			args.End = t.Add(1);
			Query::IdentifyFrames(experiment,
			                      [&identifieds] (const IdentifiedFrame::Ptr & i) {
				                      identifieds.push_back(i);
			                      },
			                      args);
		});
	EXPECT_EQ(identifieds.size(),1);

	identifieds.clear();
	try {
		myrmidon::Query::IdentifyFramesArgs args;
		args.Start = t.Add(1);
		Query::IdentifyFrames(experiment,
		                      [&identifieds] (const IdentifiedFrame::Ptr & i) {
			                      identifieds.push_back(i);
		                      },
		                      args);
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Unexpected exception: " << e.what();
		return;
	}
	// we just have removed the first frame
	EXPECT_EQ(identifieds.size(),599);
}

TEST_F(QueryUTest,InteractionFrame) {
	ASSERT_NO_THROW({
			auto a1 = experiment->CreateAnt(1);
			auto a2 = experiment->CreateAnt(2);
			Identifier::AddIdentification(experiment->Identifier(),1,123,Time::SinceEver(),Time::Forever());
			Identifier::AddIdentification(experiment->Identifier(),2,124,Time::SinceEver(),Time::Forever());
			experiment->CreateAntShapeType("body",1);

			for ( const auto & ant : {a1,a2} ) {
				ant->AddCapsule(1,std::make_shared<Capsule>(Eigen::Vector2d(0,10),
				                                            Eigen::Vector2d(0,-10),
				                                            10,10));
			}
		});

	std::vector<Query::CollisionData> collisionData;

	ASSERT_NO_THROW({
			myrmidon::Query::QueryArgs args;
			Query::CollideFrames(experiment,
			                     [&collisionData] (const Query::CollisionData & data) {
				                     collisionData.push_back(data);
			                     },
			                     args);
		});

	ASSERT_EQ(collisionData.size(),600);

	size_t nonEmptyFrame(0);
	for ( const auto & [positions,collisions] : collisionData ) {
		EXPECT_EQ(positions->Space,1);
		EXPECT_EQ(collisions->Space,1);
		if ( collisions->Collisions.empty() == false ) {
			++nonEmptyFrame;
		}
	}
	EXPECT_EQ(nonEmptyFrame,156);

}


TEST_F(QueryUTest,TrajectoryComputation) {
	ASSERT_NO_THROW({
			experiment->CreateAnt(1);
			Identifier::AddIdentification(experiment->Identifier(),1,123,Time::SinceEver(),Time::Forever());
		});

	std::vector<AntTrajectory::Ptr> trajectories;

	ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntTrajectoriesArgs args;
			args.MaximumGap = 20000 * Duration::Millisecond;
			Query::ComputeTrajectories(experiment,
			                           [&trajectories]( const AntTrajectory::Ptr & t) {
				                           trajectories.push_back(t);
			                           },
			                           args);
		});

	ASSERT_EQ(trajectories.size(),3);

	for( const auto & trajectory : trajectories ) {
		EXPECT_EQ(trajectory->Ant,1);
		EXPECT_EQ(trajectory->Space,1);
		ASSERT_EQ(trajectory->Positions.rows(),200);
		EXPECT_EQ(trajectory->Positions(0,0),0);
	}

}

TEST_F(QueryUTest,InteractionComputation) {
	ASSERT_NO_THROW({
			auto a1 = experiment->CreateAnt(1);
			auto a2 = experiment->CreateAnt(2);
			Identifier::AddIdentification(experiment->Identifier(),1,123,Time::SinceEver(),Time::Forever());
			Identifier::AddIdentification(experiment->Identifier(),2,124,Time::SinceEver(),Time::Forever());
			experiment->CreateAntShapeType("body",1);

			for ( const auto & ant : {a1,a2} ) {
				ant->AddCapsule(1,std::make_shared<Capsule>(Eigen::Vector2d(0,10),
				                                            Eigen::Vector2d(0,-10),
				                                            10,10));
			}
		});

	std::vector<AntTrajectory::Ptr> trajectories;
	std::vector<AntInteraction::Ptr> interactions;
	ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntInteractionsArgs args;
			args.MaximumGap = 220 * Duration::Millisecond;

			Query::ComputeAntInteractions(experiment,
			                              [&trajectories]( const AntTrajectory::Ptr & t) {
				                              trajectories.push_back(t);
			                              },
			                              [&interactions]( const AntInteraction::Ptr & i) {
				                              interactions.push_back(i);
			                              },
			                              args);
		});


	EXPECT_EQ(trajectories.size(),6);
	EXPECT_EQ(interactions.size(),15);
	for (const auto & interaction : interactions ) {
		EXPECT_EQ(interaction->IDs.first,1);
		EXPECT_EQ(interaction->IDs.second,2);
		EXPECT_EQ(interaction->Types.rows(),1);
		EXPECT_EQ(interaction->Types(0,0),1U);
		EXPECT_EQ(interaction->Types(0,1),1U);

		auto segmentStart =
			[](const AntTrajectorySegment &s) {
				Duration ellapsed = s.Trajectory->Positions(s.Begin,0) * double(Duration::Second.Nanoseconds());
				return s.Trajectory->Start.Add(ellapsed);
			};


		EXPECT_EQ(segmentStart(interaction->Trajectories.first),
		          interaction->Start);
		EXPECT_EQ(segmentStart(interaction->Trajectories.second),
		          interaction->Start);
	}
}


TEST_F(QueryUTest,FrameSelection) {
	ASSERT_NO_THROW({
			experiment->CreateAnt(1);
			Identifier::AddIdentification(experiment->Identifier(),1,123,Time::SinceEver(),Time::Forever());
		});

	auto firstDate = experiment->Spaces().at(1)->TrackingDataDirectories().front()->StartDate();

	std::vector<IdentifiedFrame::Ptr> frames;

	myrmidon::Query::IdentifyFramesArgs args;

	// issue 138, should select all frames
	args.Start = firstDate;
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::Ptr & f) {
		                      frames.push_back(f);
	                      },
	                      args);

	EXPECT_FALSE(frames.empty());
	frames.clear();

	//selects the first frame
	args.Start = firstDate;
	args.End = firstDate.Add(1);
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::Ptr & f) {
		                      frames.push_back(f);
	                      },
	                      args);

	ASSERT_EQ(frames.size(),1);
	ASSERT_EQ(frames[0]->FrameTime,firstDate);

	frames.clear();
	// won't access any
	args.Start = firstDate;
	args.End = firstDate;
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::Ptr & f) {
		                      frames.push_back(f);
	                      },
	                      args);

	ASSERT_EQ(frames.size(),0);





}


} // namespace priv
} // namespace myrmidon
} // namespace fort
