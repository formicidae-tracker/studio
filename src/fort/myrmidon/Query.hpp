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

	// Data returned by <CollideFrames>.
	typedef std::pair<IdentifiedFrame::ConstPtr,CollisionFrame::ConstPtr> CollisionData;

	// Computes all measurement for an Ant
	// @experiment the <Experiment> to query for
	// @antID the desired <Ant>
	// @mTypeID the desired measurement type
	//
	// @return a <ComputedMeasurement::List> of the Measurement for the <Ant>
	static ComputedMeasurement::List ComputeMeasurementFor(const CExperiment & experiment,
	                                                       Ant::ID antID,
	                                                       MeasurementTypeID mTypeID);

	// Computes <TagStatistics> for an experiment
	// @experiment the <Experiment> to query for
	//
	// @return the tag statistics index by <TagID>
	static TagStatistics::ByTagID ComputeTagStatistics(const CExperiment & experiment);


	// Identifies ants in frames - functor version
	// @experiment the <Experiment> to query for
	// @storeData a functor to store/convert the data
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @computeZones should compute zones for, makes computation slower
	//
	// Identifies Ants in frames, data will be reported ordered by
	// time.  This version aimed to be used by language bindings to
	// avoid large data copy.
	static void IdentifyFramesFunctor(const CExperiment & experiment,
	                                  std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
	                                  const Time::ConstPtr & start,
	                                  const Time::ConstPtr & end,
	                                  bool computeZones = false);


	// Identifies ants in frames
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @computeZones should compute zones for, makes computation slower
	//
	// Identifies Ants in frames, data will be reported ordered by time.
	static void IdentifyFrames(const CExperiment & experiment,
	                           std::vector<IdentifiedFrame::ConstPtr> & result,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end,
	                           bool computeZones = false);

	// Finds <Collision> in data frame - functor version
	// @OutputIter an output iterator to fill results
	// @experiment the <Experiment> to query for
	// @storeData a functor to store the data as it is produced
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	//
	// Finds <Collision> between ants in frames, data will be reported
	// ordered by time. This version aimed to be used by language bindings to
	// avoid large data copy.
	static void CollideFramesFunctor(const CExperiment & experiment,
	                                 std::function<void (const CollisionData & data)> storeData,
	                                 const Time::ConstPtr & start,
	                                 const Time::ConstPtr & end);


	// Finds <Collision> in data frame
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame> and <CollisionFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	//
	// Finds <Collision> between ants in frames, data will be reported
	// ordered by time.
	static void CollideFrames(const CExperiment & experiment,
	                          std::vector<CollisionData> & result,
	                          const Time::ConstPtr & start,
	                          const Time::ConstPtr & end);

	// Computes trajectories for ants - functor version
	// @experiment the <Experiment> to query for
	// @storeTrajectory a functor to store/covert the data
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @maximumGap the maximal undetected duration before cutting the
	//             trajectory in two
	// @matcher a <Matcher> to specify more precise, less memory
	//          intensive queries.
	// @computeZones enables ant zone computation, but slower query
	//
	// Computes trajectories for <Ant>. Those will be reported ordered
	// by ending time. This version aimed to be used by language bindings to
	// avoid large data copy.
	static void ComputeTrajectoriesFunctor(const CExperiment & experiment,
	                                       std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
	                                       const Time::ConstPtr & start,
	                                       const Time::ConstPtr & end,
	                                       Duration maximumGap,
	                                       Matcher::Ptr matcher = Matcher::Ptr(),
	                                       bool computeZones = false);



	// Computes trajectories for ants
	// @experiment the <Experiment> to query for
	// @trajectories the resulting <IdentifiedFrame> and <CollisionFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @maximumGap the maximal undetected duration before cutting the
	//             trajectory in two
	// @matcher a <Matcher> to specify more precise, less memory
	//          intensive queries.
	// @computeZones enables ant zone computation, but slower query
	//
	// Computes trajectories for <Ant>. Those will be reported ordered
	// by ending time
	static void ComputeTrajectories(const CExperiment & experiment,
	                                std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                const Time::ConstPtr & start,
	                                const Time::ConstPtr & end,
	                                Duration maximumGap,
	                                Matcher::Ptr matcher = Matcher::Ptr(),
	                                bool computeZones = false );


	// Computes interactions for ants - functor version
	// @experiment the <Experiment> to query for
	// @storeTrajectory a functor to store/convert trajectories
	// @storeInteraction  a functor to store/convert interaction
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @maximumGap the maximal undetected duration before cutting the
	//             trajectory in two
	// @matcher a <Matcher> to specify more precise, less memory
	//          intensive queries.
	//
	// Computes interactions for <Ant>. Those will be reported ordered
	// by ending time. This version aimed to be used by language bindings to
	// avoid large data copy.
	static void ComputeAntInteractionsFunctor(const CExperiment & experiment,
	                                          std::function<void ( const AntTrajectory::ConstPtr&)> storeTrajectory,
	                                          std::function<void ( const AntInteraction::ConstPtr&)> storeInteraction,
	                                          const Time::ConstPtr & start,
	                                          const Time::ConstPtr & end,
	                                          Duration maximumGap,
	                                          Matcher::Ptr matcher = Matcher::Ptr());



	// Computes interactions for ants
	// @experiment the <Experiment> to query for
	// @trajectories the resulting <IdentifiedFrame> and <CollisionFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @maximumGap the maximal undetected duration before cutting the
	//             trajectory in two
	// @matcher a <Matcher> to specify more precise, less memory
	//          intensive queries.
	//
	// Computes interactions for <Ant>. Those will be reported ordered
	// by ending time.
	static void ComputeAntInteractions(const CExperiment & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   std::vector<AntInteraction::ConstPtr> & interactions,
	                                   const Time::ConstPtr & start,
	                                   const Time::ConstPtr & end,
	                                   Duration maximumGap,
	                                   Matcher::Ptr matcher = Matcher::Ptr());


};


} // namespace myrmidon
} // namespace fort
