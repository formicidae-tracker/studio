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
	                           std::function<void (const IdentifiedFrame::Ptr &)> storeData,
	                           const myrmidon::Query::IdentifyFramesArgs & args);

	static void CollideFrames(const Experiment::ConstPtr & experiment,
	                          std::function<void (const CollisionData & data) > storeData,
	                          const myrmidon::Query::QueryArgs & args);

	static void ComputeTrajectories(const Experiment::ConstPtr & experiment,
	                                std::function<void (const AntTrajectory::Ptr &)> storeData,
	                                const myrmidon::Query::ComputeAntTrajectoriesArgs & args);


	// computes trajectories and interactions. Bad invariant
	// optimization: interactions will always be saved before
	// trajectories. But there are no test.
	static void ComputeAntInteractions(const Experiment::ConstPtr & experiment,
	                                   std::function<void (const AntTrajectory::Ptr &)> storeTrajectory,
	                                   std::function<void (const AntInteraction::Ptr &)> storeInteraction,
	                                   const myrmidon::Query::ComputeAntInteractionsArgs & args);
private:

};


} // namespace priv
} // namespace myrmidon
} // namespace fort


#include "Query.impl.hpp"
