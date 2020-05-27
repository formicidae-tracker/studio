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


void Query::IdentifyFrames(const CExperiment & experiment,
                           std::vector<IdentifiedFrame::ConstPtr> & result,
                           const Time::ConstPtr & start,
                           const Time::ConstPtr & end) {
	auto backInserter = std::back_inserter(result);
	priv::Query::IdentifyFrames(experiment.d_p,backInserter,start,end);
}


void Query::CollideFrames(const CExperiment & experiment,
                         std::vector<CollisionData> & result,
                         const Time::ConstPtr & start,
                         const Time::ConstPtr & end) {
	auto backInserter = std::back_inserter(result);
	priv::Query::CollideFrames(experiment.d_p,backInserter,start,end);
}

void Query::ComputeTrajectories(const CExperiment & experiment,
                                std::vector<AntTrajectory::ConstPtr> & trajectories,
                                const Time::ConstPtr & start,
                                const Time::ConstPtr & end,
                                Duration maximumGap,
                                Matcher::Ptr matcher) {
	auto backInserter = std::back_inserter(trajectories);
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 backInserter,
	                                 start,
	                                 end,
	                                 maximumGap,
	                                 matcher->d_p);
}

void Query::ComputeAntInteractions(const CExperiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   std::vector<AntInteraction::ConstPtr> & interactions,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end,
                                   Duration maximumGap,
                                   Matcher::Ptr matcher) {
	auto trajInserter = std::back_inserter(trajectories);
	auto interInserter = std::back_inserter(interactions);

	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    trajInserter,
	                                    interInserter,
	                                    start,
	                                    end,
	                                    maximumGap,
	                                    matcher->d_p);
}


} // namespace myrmidon
} // namespace fort
