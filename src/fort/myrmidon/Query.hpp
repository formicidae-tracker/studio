#pragma once

#include "Types.hpp"

#include "Experiment.hpp"
#include "Matchers.hpp"

namespace fort {
namespace myrmidon {


// Wrapper for Queries on Experiment
//
// This class is a wrapper for all data queries that can be made on an
// Experiment. They takes advantages of multithreading to have
// efficient computation time.
class Query {
public:
	typedef std::pair<IdentifiedFrame::ConstPtr,CollisionFrame::ConstPtr> CollisionData;

	// Computes all measurement for an Ant
	// @experiment the <Experiment> to query for
	// @antID the desired <Ant>
	// @mTypeID the desired measurement type
	//
	// @return a <ComputedMeasurement::List> of the Measurement for the <Ant>
	static ComputedMeasurement::List ComputeMeasurementFor(const Experiment::ConstPtr & experiment,
	                                                       Ant::ID antID,
	                                                       MeasurementTypeID mTypeID);

	static TagStatistics::ByTagID ComputeTagStatistics(const Experiment::ConstPtr & experiment);

	static void IdentifyFrames(const Experiment::ConstPtr & experiment,
	                           std::vector<IdentifiedFrame::ConstPtr> & result,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end);

	static void CollideFrame(const Experiment::ConstPtr & experiment,
	                         std::vector<CollisionData> & result,
	                         const Time::ConstPtr & start,
	                         const Time::ConstPtr & end);

	static void ComputeTrajectories(const Experiment::ConstPtr & experiment,
	                                std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                const Time::ConstPtr & start,
	                                const Time::ConstPtr & end,
	                                Duration maximumGap,
	                                Matcher::Ptr matcher = Matcher::Ptr());

	static void ComputeAntInteractions(const Experiment::ConstPtr & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   std::vector<AntInteraction::ConstPtr> & interactions,
	                                   const Time::ConstPtr & start,
	                                   const Time::ConstPtr & end,
	                                   Duration maximumGap,
	                                   Matcher::Ptr matcher = Matcher::Ptr());


};


} // namespace myrmidon
} // namespace fort
