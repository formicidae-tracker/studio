#include "Query.hpp"

#include <tbb/pipeline.h>

#include "priv/Query.hpp"



namespace fort {
namespace myrmidon {


ComputedMeasurement::List Query::ComputeMeasurementFor(const Experiment & experiment,
                                                       Ant::ID antID,
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
	: Start(Time::SinceEver())
	, End(Time::Forever())
	, ComputeZones(false)
	, SingleThreaded(false) {
}

void Query::IdentifyFramesFunctor(const Experiment & experiment,
                                  std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
                                  const IdentifyFramesArgs & args) {
	priv::Query::IdentifyFrames(experiment.d_p,storeData,args);
}


void Query::IdentifyFrames(const Experiment & experiment,
                           std::vector<IdentifiedFrame::ConstPtr> & result,
                           const IdentifyFramesArgs & args) {
	priv::Query::IdentifyFrames(experiment.d_p,
	                            [&result] (const IdentifiedFrame::ConstPtr & i) {
		                            result.push_back(i);
	                            },
	                            args);
}

Query::CollideFramesArgs::CollideFramesArgs()
	: Start(Time::SinceEver())
	, End(Time::Forever())
	, SingleThreaded(false) {
}

void Query::CollideFramesFunctor(const Experiment & experiment,
                                 std::function<void (const CollisionData & data)> storeData,
                                 const CollideFramesArgs & args) {
	priv::Query::CollideFrames(experiment.d_p,storeData,args);
}


void Query::CollideFrames(const Experiment & experiment,
                          std::vector<CollisionData> & result,
                          const CollideFramesArgs & args) {
	priv::Query::CollideFrames(experiment.d_p,
	                           [&result](const CollisionData & data) {
		                           result.push_back(data);
	                           },
	                           args);
}

Query::ComputeAntTrajectoriesArgs::ComputeAntTrajectoriesArgs()
	: Start(Time::SinceEver())
	, End(Time::Forever())
	, MaximumGap(1 * Duration::Second)
	, ComputeZones(false)
	, SingleThreaded(false) {
}


void Query::ComputeAntTrajectoriesFunctor(const Experiment & experiment,
                                          std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
                                          const ComputeAntTrajectoriesArgs & args) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 storeTrajectory,
	                                 args);
}


void Query::ComputeAntTrajectories(const Experiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   const ComputeAntTrajectoriesArgs & args) {
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 [&trajectories](const AntTrajectory::ConstPtr & trajectory) {
		                                 trajectories.push_back(trajectory);
	                                 },
	                                 args);
}

Query::ComputeAntInteractionsArgs::ComputeAntInteractionsArgs()
	: Start(Time::SinceEver())
	, End(Time::Forever())
	, MaximumGap(1 * Duration::Second)
	, SingleThreaded(false)
	, ReportFullTrajectories(true) {
}

void Query::ComputeAntInteractionsFunctor(const Experiment & experiment,
                                          std::function<void ( const AntTrajectory::ConstPtr&) > storeTrajectory,
                                          std::function<void ( const AntInteraction::ConstPtr&) > storeInteraction,
                                          const ComputeAntInteractionsArgs & args) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    storeTrajectory,
	                                    storeInteraction,
	                                    args);
}

void Query::ComputeAntInteractions(const Experiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   std::vector<AntInteraction::ConstPtr> & interactions,
                                   const ComputeAntInteractionsArgs & args) {
	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    [&trajectories](const AntTrajectory::ConstPtr & trajectory) {
		                                    trajectories.push_back(trajectory);
	                                    },
	                                    [&interactions](const AntInteraction::ConstPtr & interaction) {
		                                    interactions.push_back(interaction);
	                                    },
	                                    args);
}


} // namespace myrmidon
} // namespace fort
