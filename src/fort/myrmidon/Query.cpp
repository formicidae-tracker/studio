#include "Query.hpp"

#include <tbb/pipeline.h>

#include "priv/Query.hpp"



namespace fort {
namespace myrmidon {


ComputedMeasurement::List Query::ComputeMeasurementFor(const CExperiment & experiment,
                                                       Ant::ID antID,
                                                       MeasurementTypeID mTypeID) {
	ComputedMeasurement::List res;
	experiment.d_p->ComputeMeasurementsForAnt(res,antID,mTypeID);
	return res;
}

TagStatistics::ByTagID Query::ComputeTagStatistics(const CExperiment & experiment) {
	TagStatistics::ByTagID res;
	priv::Query::ComputeTagStatistics(experiment.d_p,res);
	return res;
}


void Query::IdentifyFramesFunctor(const CExperiment & experiment,
                                  std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
                                  const Time & start,
                                  const Time & end,
                                  bool computeZones,
                                  bool singleThreaded) {
	priv::Query::IdentifyFrames(experiment.d_p,storeData,start,end,computeZones,singleThreaded);
}


void Query::IdentifyFrames(const CExperiment & experiment,
                           std::vector<IdentifiedFrame::ConstPtr> & result,
                           const Time & start,
                           const Time & end,
                           bool computeZones,
                           bool singleThread) {
	priv::Query::IdentifyFrames(experiment.d_p,
	                            [&result] (const IdentifiedFrame::ConstPtr & i) {
		                            result.push_back(i);
	                            },
	                            start,end,computeZones,singleThread);
}


void Query::CollideFramesFunctor(const CExperiment & experiment,
                                 std::function<void (const CollisionData & data)> storeData,
                                 const Time & start,
                                 const Time & end,
                                 bool singleThread) {
	priv::Query::CollideFrames(experiment.d_p,storeData,start,end,singleThread);
}


void Query::CollideFrames(const CExperiment & experiment,
                          std::vector<CollisionData> & result,
                          const Time & start,
                          const Time & end,
                          bool singleThread) {
	priv::Query::CollideFrames(experiment.d_p,
	                           [&result](const CollisionData & data) {
		                           result.push_back(data);
	                           },
	                           start,end,singleThread);
}

void Query::ComputeAntTrajectoriesFunctor(const CExperiment & experiment,
                                          std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
                                          const Time & start,
                                          const Time & end,
                                          Duration maximumGap,
                                          const Matcher::Ptr & matcher,
                                          bool computeZones,
                                          bool singleThread) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 storeTrajectory,
	                                 start,
	                                 end,
	                                 maximumGap,
	                                 !matcher ? Matcher::PPtr() : matcher->d_p,
	                                 computeZones,
	                                 singleThread);
}



void Query::ComputeAntTrajectories(const CExperiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   const Time & start,
                                   const Time & end,
                                   Duration maximumGap,
                                   const Matcher::Ptr & matcher,
                                   bool computeZones,
                                   bool singleThread) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 [&trajectories](const AntTrajectory::ConstPtr & trajectory) {
		                                 trajectories.push_back(trajectory);
	                                 },
	                                 start,
	                                 end,
	                                 maximumGap,
	                                 !matcher ? Matcher::PPtr() : matcher->d_p,
	                                 computeZones,
	                                 singleThread);
}

void Query::ComputeAntInteractionsFunctor(const CExperiment & experiment,
                                          std::function<void ( const AntTrajectory::ConstPtr&) > storeTrajectory,
                                          std::function<void ( const AntInteraction::ConstPtr&) > storeInteraction,
                                          const Time & start,
                                          const Time & end,
                                          Duration maximumGap,
                                          const Matcher::Ptr & matcher,
                                          bool singleThread) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    storeTrajectory,
	                                    storeInteraction,
	                                    start,
	                                    end,
	                                    maximumGap,
	                                    !matcher ? Matcher::PPtr() : matcher->d_p,
	                                    singleThread);
}


void Query::ComputeAntInteractions(const CExperiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   std::vector<AntInteraction::ConstPtr> & interactions,
                                   const Time & start,
                                   const Time & end,
                                   Duration maximumGap,
                                   const Matcher::Ptr & matcher,
                                   bool singleThread) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    [&trajectories](const AntTrajectory::ConstPtr & trajectory) {
		                                    trajectories.push_back(trajectory);
	                                    },
	                                    [&interactions](const AntInteraction::ConstPtr & interaction) {
		                                    interactions.push_back(interaction);
	                                    },
	                                    start,
	                                    end,
	                                    maximumGap,
	                                    !matcher ? Matcher::PPtr() : matcher->d_p,
	                                    singleThread);
}


} // namespace myrmidon
} // namespace fort
