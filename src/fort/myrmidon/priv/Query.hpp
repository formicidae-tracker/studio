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
	                           std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end,
	                           bool computeZones = false,
	                           bool singleThreaded = false);

	static void CollideFrames(const Experiment::ConstPtr & experiment,
	                          std::function<void (const CollisionData & data) > storeData,
	                          const Time::ConstPtr & start,
	                          const Time::ConstPtr & end,
	                          bool singleThreaded = false);

	static void ComputeTrajectories(const Experiment::ConstPtr & experiment,
	                                std::function<void (const AntTrajectory::ConstPtr &)> storeData,
	                                const Time::ConstPtr & start,
	                                const Time::ConstPtr & end,
	                                Duration maximumGap,
	                                const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                bool computeZones = false,
	                                bool singleThreaded = false);


	// computes trajectories and interactions. Bad invariant
	// optimization: interactions will always be saved before
	// trajectories. But there are no test.
	static void ComputeAntInteractions(const Experiment::ConstPtr & experiment,
	                                   std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
	                                   std::function<void (const AntInteraction::ConstPtr &)> storeInteraction,
	                                   const Time::ConstPtr & start,
	                                   const Time::ConstPtr & end,
	                                   Duration maximumGap,
	                                   const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                   bool singleThreaded = false);

private:
	typedef std::pair<TrackingDataDirectory::const_iterator,
	                  TrackingDataDirectory::const_iterator> DataRange;
	typedef std::map<Space::ID,std::vector<DataRange>>       DataRangeBySpaceID;
	typedef std::pair<Space::ID,RawFrameConstPtr>            RawData;

	struct BuildingTrajectory {
		std::shared_ptr<AntTrajectory> Trajectory;

		Time                  Last;
		std::vector<double>   DataPoints;
		std::vector<double>   Durations;
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
		std::set<std::pair<AntShapeTypeID,AntShapeTypeID>> Types;
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
	                       DataRangeBySpaceID & ranges);

	class DataLoader {
	public:
		DataLoader(const DataRangeBySpaceID & dataRanges);

		RawData operator()( tbb::flow_control & fc) const;
		RawData operator()() const;
	private:
		typedef std::map<Space::ID,std::vector<DataRange>::const_iterator> RangesIteratorByID;
		typedef std::map<Space::ID,TrackingDataDirectory::const_iterator> DataIteratorByID;
		const DataRangeBySpaceID & d_dataRanges;
		std::shared_ptr<RangesIteratorByID> d_rangeIterators;
		std::shared_ptr<DataIteratorByID>   d_dataIterators;

	};


	static std::function<void(const IdentifiedFrame::ConstPtr &)>
	BuildTrajectories(std::function<void(const AntTrajectory::ConstPtr&)> store,
	                  BuildingTrajectoryData & building,
	                  Duration maxGap,
	                  const Matcher::Ptr & matcher);


	static std::function<void(const CollisionData &)>
	BuildInteractions(std::function<void(const AntTrajectory::ConstPtr&)> storeTrajectory,
	                  std::function<void(const AntInteraction::ConstPtr&)> storeInteraction,
	                  BuildingTrajectoryData & currentTrajectories,
	                  BuildingInteractionData & currentInteractions,
	                  Duration maxGap,
	                  const Matcher::Ptr & matcher);



};


} // namespace priv
} // namespace myrmidon
} // namespace fort


#include "Query.impl.hpp"
