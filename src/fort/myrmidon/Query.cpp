#include "Query.hpp"

#include <tbb/pipeline.h>

#include "priv/Query.hpp"
namespace fort {
namespace myrmidon {

ComputedMeasurement::List Query::ComputeMeasurementFor(const Experiment & experiment,
                                                       AntID antID,
                                                       MeasurementTypeID mTypeID) {
	ComputedMeasurement::List res;
	experiment.d_p->ComputeMeasurementsForAnt(res,antID,mTypeID);
	return res;
}

TagStatistics::ByTagID Query::ComputeTagStatistics(const Experiment & experiment) {
	TagStatistics::ByTagID res;
	priv::Query::ComputeTagStatistics(experiment.d_p,res);
	return res;
}

Query::IdentifyFramesArgs::IdentifyFramesArgs()
	: ComputeZones(false) {
}

void Query::IdentifyFramesFunctor(const Experiment & experiment,
                                  std::function<void (const IdentifiedFrame::Ptr &)> storeData,
                                  const IdentifyFramesArgs & args) {
	priv::Query::IdentifyFrames(experiment.d_p,storeData,args);
}


void Query::IdentifyFrames(const Experiment & experiment,
                           std::vector<IdentifiedFrame::Ptr> & result,
                           const IdentifyFramesArgs & args) {
	priv::Query::IdentifyFrames(experiment.d_p,
	                            [&result] (const IdentifiedFrame::Ptr & i) {
		                            result.push_back(i);
	                            },
	                            args);
}

Query::QueryArgs::QueryArgs()
	: Start(Time::SinceEver())
	, End(Time::Forever())
	, SingleThreaded(false)
	, AllocationInCurrentThread(false) {
}

void Query::CollideFramesFunctor(const Experiment & experiment,
                                 std::function<void (const CollisionData & data)> storeData,
                                 const QueryArgs & args) {
	priv::Query::CollideFrames(experiment.d_p,storeData,args);
}


void Query::CollideFrames(const Experiment & experiment,
                          std::vector<CollisionData> & result,
                          const QueryArgs & args) {
	priv::Query::CollideFrames(experiment.d_p,
	                           [&result](const CollisionData & data) {
		                           result.push_back(data);
	                           },
	                           args);
}

Query::ComputeAntTrajectoriesArgs::ComputeAntTrajectoriesArgs()
	: MaximumGap(1 * Duration::Second)
	, ComputeZones(false) {
}


void Query::ComputeAntTrajectoriesFunctor(const Experiment & experiment,
                                          std::function<void (const AntTrajectory::Ptr &)> storeTrajectory,
                                          const ComputeAntTrajectoriesArgs & args) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 storeTrajectory,
	                                 args);
}


void Query::ComputeAntTrajectories(const Experiment & experiment,
                                   std::vector<AntTrajectory::Ptr> & trajectories,
                                   const ComputeAntTrajectoriesArgs & args) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 [&trajectories](const AntTrajectory::Ptr & trajectory) {
		                                 trajectories.push_back(trajectory);
	                                 },
	                                 args);
}

Query::ComputeAntInteractionsArgs::ComputeAntInteractionsArgs()
	: MaximumGap(1 * Duration::Second)
	, ReportFullTrajectories(true) {
}

void Query::ComputeAntInteractionsFunctor(const Experiment & experiment,
                                          std::function<void ( const AntTrajectory::Ptr&) > storeTrajectory,
                                          std::function<void ( const AntInteraction::Ptr&) > storeInteraction,
                                          const ComputeAntInteractionsArgs & args) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    storeTrajectory,
	                                    storeInteraction,
	                                    args);
}

void Query::ComputeAntInteractions(const Experiment & experiment,
                                   std::vector<AntTrajectory::Ptr> & trajectories,
                                   std::vector<AntInteraction::Ptr> & interactions,
                                   const ComputeAntInteractionsArgs & args) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    [&trajectories](const AntTrajectory::Ptr & trajectory) {
		                                    trajectories.push_back(trajectory);
	                                    },
	                                    [&interactions](const AntInteraction::Ptr & interaction) {
		                                    interactions.push_back(interaction);
	                                    },
	                                    args);
}


} // namespace myrmidon
} // namespace fort
