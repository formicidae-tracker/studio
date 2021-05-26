#pragma once

#include <functional>

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Query.hpp>

#include "Matchers.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class DataSegmenter {
public:
	struct Args {
		std::function<void (AntTrajectory::ConstPtr &)> StoreTrajectory;
		std::function<void (AntInteraction::ConstPtr &)> StoreInteraction;

		Duration            MaximumGap;
		priv::Matcher::Ptr  Matcher;
		bool                SummarizeSegment;
	};

	DataSegmenter(const Args & args);
	~DataSegmenter();

	void operator()(const Query::CollisionData & data);


private:
	struct BuildingTrajectory {
		typedef std::shared_ptr<BuildingTrajectory> Ptr;
		std::shared_ptr<AntTrajectory> Trajectory;

		Time                  Last;
		std::vector<double>   DataPoints;
		std::vector<double>   Durations;
		std::vector<uint32_t> Zones;
		bool                  ForceKeep;

		BuildingTrajectory(const IdentifiedFrame::ConstPtr & frame,
		                   const PositionedAnt & ant,
		                   const ZoneID * zone);

		void Append(const IdentifiedFrame::ConstPtr & frame,
		            const PositionedAnt & ant,
		            const ZoneID * zone);

		AntTrajectory::ConstPtr Terminate() const;
	};


	struct BuildingInteraction {
		InteractionID IDs;
		Time          Start,Last;

		std::pair<size_t,size_t>                                   SegmentStarts;
		std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> Trajectories;

		std::set<std::pair<AntShapeTypeID,AntShapeTypeID>> Types;

		BuildingInteraction(const Collision & collision,
		                    const Time & curTime,
							std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> trajectories);

		void Append(const Collision & collision,
		            const Time & curTime);


		static void SummarizeTrajectorySegment(AntTrajectorySegment & s);

		AntInteraction::ConstPtr Terminate(bool summarize) const;
	};


	void BuildTrajectories(const IdentifiedFrame::ConstPtr & identified,
						   bool conserveAllTrajectory);

	void TerminateTrajectory(const BuildingTrajectory::Ptr & trajectory);

	void BuildInteractions(const CollisionFrame::ConstPtr & collisions);



	std::map<uint32_t,BuildingTrajectory::Ptr>  d_trajectories;
	std::map<InteractionID,BuildingInteraction> d_interactions;
	Args                                        d_args;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort