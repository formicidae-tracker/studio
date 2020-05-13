#pragma once

#include <fort/myrmidon/Types.hpp>

#include "Experiment.hpp"
#include "TrackingDataDirectory.hpp"
#include "Matchers.hpp"


#include <tbb/pipeline.h>

namespace fort {
namespace myrmidon {
namespace priv {


class Query {
public:
	typedef std::pair<Space::ID,IdentifiedFrame::ConstPtr>                             IdentifiedData;
	typedef std::tuple<Space::ID,IdentifiedFrame::ConstPtr,InteractionFrame::ConstPtr> InteractionData;


	static void ComputeTagStatistics(const Experiment::ConstPtr & experiment,
	                                 TagStatistics::ByTagID & result);

	static void IdentifyFrames(const Experiment::ConstPtr & experiment,
	                           std::vector<IdentifiedData> & result,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end);

	static void InteractFrame(const Experiment::ConstPtr & experiment,
	                          std::vector<InteractionData> & result,
	                          const Time::ConstPtr & start,
	                          const Time::ConstPtr & end);

	static void ComputeTrajectories(const Experiment::ConstPtr & experiment,
	                                std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                const Time::ConstPtr & start,
	                                const Time::ConstPtr & end,
	                                Duration maximumGap,
	                                Matcher::Ptr matcher = Matcher::Any());

	static void ComputeAntInteractions(const Experiment::ConstPtr & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   std::vector<AntInteraction::ConstPtr> & interactions,
	                                   const Time::ConstPtr & start,
	                                   const Time::ConstPtr & end,
	                                   Duration maximumGap,
	                                   Matcher::Ptr matcher = Matcher::Any());


private:
	typedef std::pair<TrackingDataDirectory::const_iterator,
	                  TrackingDataDirectory::const_iterator> DataRange;
	typedef std::vector<std::pair<Space::ID,DataRange>>      DataRangeWithSpace;
	typedef std::pair<Space::ID,RawFrameConstPtr>            RawData;

	struct BuildingTrajectory {
		Space::ID SpaceID;
		Time Start,Last;
		std::vector<double> DataPoints;
		std::vector<uint64_t> Durations;
		AntTrajectory::ConstPtr Terminate(AntID antID) const;
	};

	struct BuildingInteraction {
		InteractionID             IDs;
		Time Start,Last;
		std::set<InteractionType> Types;
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

	static std::function<void(const IdentifiedData &)>
	BuildTrajectories(std::vector<AntTrajectory::ConstPtr> & result,
	                  BuildingTrajectoryData & building,
	                  Duration maxGap,
	                  const Matcher::Ptr & matcher);


	static std::function<void(const InteractionData &)>
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
