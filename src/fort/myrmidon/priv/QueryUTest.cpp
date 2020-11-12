#include "QueryUTest.hpp"

#include "Query.hpp"
#include "Ant.hpp"
#include "Capsule.hpp"

#include <fort/myrmidon/TestSetup.hpp>

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
	                      Time::Parse("2019-11-02T09:05:48.908406Z")));

	EXPECT_EQ(tagStats.at(123).ID,123);

	EXPECT_EQ(tagStats.at(123).Counts(TagStatistics::TOTAL_SEEN),3000);
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
			Identifier::AddIdentification(experiment->Identifier(),1,123,{},{});
		});

	std::vector<IdentifiedFrame::ConstPtr> identifieds;

	ASSERT_NO_THROW({
			Query::IdentifyFrames(experiment,
			                      [&identifieds] (const IdentifiedFrame::ConstPtr & i) {
				                      identifieds.push_back(i);
			                      },
			                      {},
			                      {});
		});
	ASSERT_EQ(identifieds.size(),3000);
	for ( const auto & frame : identifieds ) {
		EXPECT_EQ(frame->Space,1);
		ASSERT_EQ(frame->Positions.size(),1);
		ASSERT_EQ(frame->Positions[0].ID,1);
	}

	auto t = experiment->CSpaces().begin()->second->TrackingDataDirectories().front()->StartDate();
	identifieds.clear();
	ASSERT_NO_THROW({
			Query::IdentifyFrames(experiment,
			                      [&identifieds] (const IdentifiedFrame::ConstPtr & i) {
				                      identifieds.push_back(i);
			                      },
			                      {},
			                      std::make_shared<Time>(t.Add(1)));
		});
	EXPECT_EQ(identifieds.size(),1);

	identifieds.clear();
	try {
		Query::IdentifyFrames(experiment,
		                      [&identifieds] (const IdentifiedFrame::ConstPtr & i) {
			                      identifieds.push_back(i);
		                      },
		                      std::make_shared<Time>(t.Add(1)),
		                      {});
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Unexpected exception: " << e.what();
		return;
	}
	// we just have removed the first frame
	EXPECT_EQ(identifieds.size(),2999);
}

TEST_F(QueryUTest,InteractionFrame) {
	ASSERT_NO_THROW({
			auto a1 = experiment->CreateAnt(1);
			auto a2 = experiment->CreateAnt(2);
			Identifier::AddIdentification(experiment->Identifier(),1,123,{},{});
			Identifier::AddIdentification(experiment->Identifier(),2,124,{},{});
			experiment->CreateAntShapeType("body",1);

			for ( const auto & ant : {a1,a2} ) {
				ant->AddCapsule(1,Capsule(Eigen::Vector2d(0,10),
				                          Eigen::Vector2d(0,-10),
				                          10,10));
			}
		});

	std::vector<Query::CollisionData> collisionData;

	ASSERT_NO_THROW({
			Query::CollideFrames(experiment,
			                     [&collisionData] (const Query::CollisionData & data) {
				                     collisionData.push_back(data);
			                     },
			                     {},{});
		});

	ASSERT_EQ(collisionData.size(),3000);

	size_t nonEmptyFrame(0);
	for ( const auto & [positions,collisions] : collisionData ) {
		EXPECT_EQ(positions->Space,1);
		EXPECT_EQ(collisions->Space,1);
		if ( collisions->Collisions.empty() == false ) {
			++nonEmptyFrame;
		}
	}
	EXPECT_EQ(nonEmptyFrame,780);

}


TEST_F(QueryUTest,TrajectoryComputation) {
	ASSERT_NO_THROW({
			experiment->CreateAnt(1);
			Identifier::AddIdentification(experiment->Identifier(),1,123,{},{});
		});

	std::vector<AntTrajectory::ConstPtr> trajectories;

	ASSERT_NO_THROW({
			Query::ComputeTrajectories(experiment,
			                           [&trajectories]( const AntTrajectory::ConstPtr & t) {
				                           trajectories.push_back(t);
			                           },
			                           {},
			                           {},
			                           20000 * Duration::Millisecond,
			                           {});
		});

	ASSERT_EQ(trajectories.size(),3);

	for( const auto & trajectory : trajectories ) {
		EXPECT_EQ(trajectory->Ant,1);
		EXPECT_EQ(trajectory->Space,1);
		ASSERT_EQ(trajectory->Positions.rows(),1000);
		EXPECT_EQ(trajectory->Positions(0,0),0);
	}

}

TEST_F(QueryUTest,InteractionComputation) {
	ASSERT_NO_THROW({
			auto a1 = experiment->CreateAnt(1);
			auto a2 = experiment->CreateAnt(2);
			Identifier::AddIdentification(experiment->Identifier(),1,123,{},{});
			Identifier::AddIdentification(experiment->Identifier(),2,124,{},{});
			experiment->CreateAntShapeType("body",1);

			for ( const auto & ant : {a1,a2} ) {
				ant->AddCapsule(1,Capsule(Eigen::Vector2d(0,10),
				                          Eigen::Vector2d(0,-10),
				                          10,10));
			}
		});

	std::vector<AntTrajectory::ConstPtr> trajectories;
	std::vector<AntInteraction::ConstPtr> interactions;
	ASSERT_NO_THROW({
			Query::ComputeAntInteractions(experiment,
			                              [&trajectories]( const AntTrajectory::ConstPtr & t) {
				                              trajectories.push_back(t);
			                              },
			                              [&interactions]( const AntInteraction::ConstPtr & i) {
				                              interactions.push_back(i);
			                              },
			                              {},
			                              {},
			                              220 * Duration::Millisecond,
			                              {});
		});


	EXPECT_EQ(trajectories.size(),6);
	EXPECT_EQ(interactions.size(),63);
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
			Identifier::AddIdentification(experiment->Identifier(),1,123,{},{});
		});

	auto firstDate = experiment->CSpaces().at(1)->TrackingDataDirectories().front()->StartDate();

	std::vector<IdentifiedFrame::ConstPtr> frames;

	// issue 138, should select all frames
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::ConstPtr & f) {
		                      frames.push_back(f);
	                      },
	                      std::make_shared<Time>(firstDate),
	                      {});

	EXPECT_FALSE(frames.empty());
	frames.clear();

	//selects the first frame
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::ConstPtr & f) {
		                      frames.push_back(f);
	                      },
	                      std::make_shared<Time>(firstDate),
	                      std::make_shared<Time>(firstDate.Add(1)));

	ASSERT_EQ(frames.size(),1);
	ASSERT_EQ(frames[0]->FrameTime,firstDate);

	frames.clear();
	// won't access any
	Query::IdentifyFrames(experiment,
	                      [&frames](const IdentifiedFrame::ConstPtr & f) {
		                      frames.push_back(f);
	                      },
	                      std::make_shared<Time>(firstDate),
	                      std::make_shared<Time>(firstDate));

	ASSERT_EQ(frames.size(),0);





}


} // namespace priv
} // namespace myrmidon
} // namespace fort
