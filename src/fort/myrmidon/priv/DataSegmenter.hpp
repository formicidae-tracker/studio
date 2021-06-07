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
		std::function<void (AntTrajectory::Ptr &)> StoreTrajectory;
		std::function<void (AntInteraction::Ptr &)> StoreInteraction;

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
		bool                  ForceKeep;

		BuildingTrajectory(const IdentifiedFrame & frame,
		                   const PositionedAntConstRef & ant);

		void Append(const IdentifiedFrame & frame,
		            const PositionedAntConstRef & ant);

		size_t Size() const;


		AntTrajectory::Ptr Terminate() const;
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

		AntInteraction::Ptr Terminate(bool summarize) const;
	};


	void BuildTrajectories(const IdentifiedFrame::Ptr & identified,
						   bool conserveAllTrajectory);

	void TerminateTrajectory(const BuildingTrajectory::Ptr & trajectory);

	void BuildInteractions(const CollisionFrame::Ptr & collisions);



	std::map<uint32_t,BuildingTrajectory::Ptr>  d_trajectories;
	std::map<InteractionID,BuildingInteraction> d_interactions;
	Args                                        d_args;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
