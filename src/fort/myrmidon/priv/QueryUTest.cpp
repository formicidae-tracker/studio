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
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir()));
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir()));
			space->AddTrackingDataDirectory(TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",TestSetup::Basedir()));
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
			auto inserter = std::back_inserter(identifieds);
			Query::IdentifyFrames(experiment,
			                      inserter,
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
			auto inserter = std::back_inserter(identifieds);
			Query::IdentifyFrames(experiment,
			                      inserter,
			                      {},
			                      std::make_shared<Time>(t.Add(1)));
		});
	EXPECT_EQ(identifieds.size(),1);

	identifieds.clear();
	ASSERT_NO_THROW({
			auto inserter = std::back_inserter(identifieds);
			Query::IdentifyFrames(experiment,
			                      inserter,
			                      std::make_shared<Time>(t.Add(1)),
			                      {});
		});
	EXPECT_EQ(identifieds.size(),999);
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
			auto inserter = std::back_inserter(collisionData);
			Query::CollideFrames(experiment,
			                     inserter,
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
			auto inserter = std::back_inserter(trajectories);
			Query::ComputeTrajectories(experiment,
			                           inserter,
			                           {},
			                           {},
			                           20000 * Duration::Millisecond,
			                           {});
		});

	ASSERT_EQ(trajectories.size(),3);

	for( const auto & trajectory : trajectories ) {
		EXPECT_EQ(trajectory->Ant,1);
		EXPECT_EQ(trajectory->Space,1);
		ASSERT_EQ(trajectory->Data.rows(),1000);
		EXPECT_EQ(trajectory->Data(0,0),0);
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
			auto trajInserter = std::back_inserter(trajectories);
			auto interInserter = std::back_inserter(interactions);
			Query::ComputeAntInteractions(experiment,
			                              trajInserter,
			                              interInserter,
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
		EXPECT_EQ(interaction->Types.size(),1);
		EXPECT_EQ(interaction->Types.front(),
		          std::make_pair(1U,1U));
		EXPECT_EQ(interaction->Trajectories.first->Start,
		          interaction->Start);
		EXPECT_EQ(interaction->Trajectories.second->Start,
		          interaction->Start);
	}
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
