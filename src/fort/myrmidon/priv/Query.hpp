#pragma once

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Query.hpp>

#include "Experiment.hpp"
#include "TrackingDataDirectory.hpp"
#include "Matchers.hpp"


#include <tbb/pipeline.h>

namespace fort {
namespace myrmidon {
namespace priv {


class Query {
public:
	typedef fort::myrmidon::Query::CollisionData  CollisionData;

	static void ComputeTagStatistics(const Experiment::ConstPtr & experiment,
	                                 TagStatistics::ByTagID & result);

	static void IdentifyFrames(const Experiment::ConstPtr & experiment,
	                           std::vector<IdentifiedFrame::ConstPtr> & result,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end,
	                           bool computeZones = false);

	static void CollideFrames(const Experiment::ConstPtr & experiment,
	                          std::vector<CollisionData> & result,
	                          const Time::ConstPtr & start,
	                          const Time::ConstPtr & end);

	static void ComputeTrajectories(const Experiment::ConstPtr & experiment,
	                                std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                const Time::ConstPtr & start,
	                                const Time::ConstPtr & end,
	                                Duration maximumGap,
	                                Matcher::Ptr matcher = Matcher::Ptr(),
	                                bool computeZones = false);

	static void ComputeAntInteractions(const Experiment::ConstPtr & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   std::vector<AntInteraction::ConstPtr> & interactions,
	                                   const Time::ConstPtr & start,
	                                   const Time::ConstPtr & end,
	                                   Duration maximumGap,
	                                   Matcher::Ptr matcher = Matcher::Ptr());


private:
	typedef std::pair<TrackingDataDirectory::const_iterator,
	                  TrackingDataDirectory::const_iterator> DataRange;
	typedef std::vector<std::pair<Space::ID,DataRange>>      DataRangeWithSpace;
	typedef std::pair<Space::ID,RawFrameConstPtr>            RawData;

	struct BuildingTrajectory {
		AntID                 Ant;
		Space::ID             SpaceID;
		Time                  Start,Last;
		std::vector<double>   DataPoints;
		std::vector<uint64_t> Durations;
		std::vector<uint32_t> Zones;
		BuildingTrajectory(const IdentifiedFrame::ConstPtr & frame,
		                   const PositionedAnt & ant,
		                   const ZoneID * zone);
		void Append(const IdentifiedFrame::ConstPtr & frame,
		            const PositionedAnt & ant,
		            const ZoneID * zone);

		AntTrajectory::ConstPtr Terminate() const;

	};

	struct BuildingInteraction {
		InteractionID             IDs;
		Time Start,Last;
		std::set<InteractionType> Types;
		BuildingInteraction(const Collision & collision,
		                    const Time & curTime);

		void Append(const Collision & collision,
		            const Time & curTime);


		AntInteraction::ConstPtr Terminate(const BuildingTrajectory & a,
		                                   const BuildingTrajectory & b) const;
	};

	typedef std::map<AntID,BuildingTrajectory> BuildingTrajectoryData;
	typedef std::map<InteractionID,BuildingInteraction> BuildingInteractionData;


	static void BuildRange(const Experiment::ConstPtr & experiment,
	                       const Time::ConstPtr & start,
	                       const Time::ConstPtr & end,
	                       DataRangeWithSpace & ranges);

	static std::function<RawData(tbb::flow_control&)>
	LoadData(const DataRangeWithSpace & ranges,
	         DataRangeWithSpace::iterator & rangeIter,
	         TrackingDataDirectory::const_iterator & dataIter);

	static std::function<void(const IdentifiedFrame::ConstPtr &)>
	BuildTrajectories(std::vector<AntTrajectory::ConstPtr> & result,
	                  BuildingTrajectoryData & building,
	                  Duration maxGap,
	                  const Matcher::Ptr & matcher);


	static std::function<void(const CollisionData &)>
	BuildInteractions(std::vector<AntTrajectory::ConstPtr> & trajectories,
	                  std::vector<AntInteraction::ConstPtr> & interactions,
	                  BuildingTrajectoryData & currentTrajectories,
	                  BuildingInteractionData & currentInteractions,
	                  Duration maxGap,
	                  const Matcher::Ptr & matcher);



};


} // namespace priv
} // namespace myrmidon
} // namespace fort
