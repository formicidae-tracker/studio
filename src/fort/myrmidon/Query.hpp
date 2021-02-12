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
//
// ## Functor version Some queries have two version, one returing to
//  one or two std::vector, and one with a storeMethod parameter. The
//  later is to be used with bindings, or when a different data
//  structure is needed.
//
// ## Note on Multithreading
//
// For very small <Experiment> in number of Ant, most of the query
// operation are IO bounded, and the multithreading overhead will
// impact performance by 40-50% in computation time, as threads are
// waiting for data inpout to come. This is the case for
// <IdentifyFrames>, <CollideFrames>, <ComputeAntTrajectories> and
// <ComputeAntInteractions>. But for very complex scenarii, with
// several hundred of individual with complex shape, multi-threading
// could be a premium. Therefore these methodd and their Functor
// counterpart have a singleThreaded option to run the query in a
// single thread and remove the multi-thread overhead. However if the
// operations are heavy because of the size of the experiment, it will
// have an impact on performances.
//
class Query {
public:

	// Data returned by <CollideFrames>.
	typedef std::pair<IdentifiedFrame::ConstPtr,CollisionFrame::ConstPtr> CollisionData;

	// Computes all measurement for an Ant
	// @experiment the <Experiment> to query for
	// @antID the desired <Ant>
	// @mTypeID the desired measurement type
	//
	// R version:
	// ```R
	// fmQueryComputeMeasurementFor(experiment,antID,mTypeID)
	// ```
	//
	// @return a <ComputedMeasurement::List> of the Measurement for the <Ant>
	static ComputedMeasurement::List ComputeMeasurementFor(const Experiment & experiment,
	                                                       Ant::ID antID,
	                                                       MeasurementTypeID mTypeID);

	// Computes <TagStatistics> for an experiment
	// @experiment the <Experiment> to query for
	//
	// R version:
	// ```R
	// fmQueryComputeTagStatistics(experiment)
	// ```
	//
	// @return the tag statistics index by <TagID>
	static TagStatistics::ByTagID ComputeTagStatistics(const Experiment & experiment);


	// Identifies ants in frames - functor version
	// @experiment the <Experiment> to query for
	// @storeData a functor to store/convert the data
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @computeZones should compute zones for, makes computation slower
	// @singleThread run this query on a single thread
	//
	// Identifies Ants in frames, data will be reported ordered by
	// time.  This version aimed to be used by language bindings to
	// avoid large data copy.
	//
	// R version:
	// ```R
	// fmQueryIdentifyFrames(experiment,
	//                       start = NULL,
	//                       end = NULL,
	//                       computeZones = FALSE,
	//                       singleThreaded = FALSE,
	//                       showProgress = FALSE)
	// ```
	static void IdentifyFramesFunctor(const Experiment & experiment,
	                                  std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
	                                  const Time & start,
	                                  const Time & end,
	                                  bool computeZones = false,
	                                  bool singleThreaded = false);


	// Identifies ants in frames
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @computeZones should compute zones for, makes computation slower
	// @singleThread run this query on a single thread
	//
	// Identifies Ants in frames, data will be reported ordered by time.
	//
	// R version:
	// ```R
	// fmQueryIdentifyFrames(experiment,
	//                       start = NULL,
	//                       end = NULL,
	//                       computeZones = FALSE,
	//                       singleThreaded = FALSE,
	//                       showProgress = FALSE)
	// ```
	static void IdentifyFrames(const Experiment & experiment,
	                           std::vector<IdentifiedFrame::ConstPtr> & result,
	                           const Time & start,
	                           const Time & end,
	                           bool computeZones = false,
	                           bool singleThreaded = false);

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
	//
	// R version:
	// ```R
	// fmQueryCollideFrames(experiment,
	//                      start = NULL,
	//                      end = NULL,
	//                      singleThreaded = FALSE,
	//                      showProgress = FALSE)
	// ```
	static void CollideFramesFunctor(const Experiment & experiment,
	                                 std::function<void (const CollisionData & data)> storeData,
	                                 const Time & start,
	                                 const Time & end,
	                                 bool singleThread = false);


	// Finds <Collision> in data frame
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame> and <CollisionFrame>
	// @start the start time for the query use nullptr for the starts
	//        of the experiment.
	// @end the end time for the query, use nullptr for the end of the
	//      experiment
	// @singleThread run this query on a single thread
	//
	// Finds <Collision> between ants in frames, data will be reported
	// ordered by time.
	//
	// R version:
	// ```R
	// fmQueryCollideFrames(experiment,
	//                      start = NULL,
	//                      end = NULL,
	//                      singleThreaded = FALSE,
	//                      showProgress = FALSE)
	// ```
	static void CollideFrames(const Experiment & experiment,
	                          std::vector<CollisionData> & result,
	                          const Time & start,
	                          const Time & end,
	                          bool singleThread = false);

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
	// @singleThread run this query on a single thread
	//
	// Computes trajectories for <Ant>. Those will be reported ordered
	// by ending time. This version aimed to be used by language bindings to
	// avoid large data copy.
	//
	// R version:
	// ```R
	// fmQueryComputeAntTrajectories(experiment,
	//                               start = NULL,
	//                               end = NULL,
	//                               maximuGap = fmSecond(1),
	//                               matcher = NULL,
	//                               computeZones = FALSE
	//                               singleThreaded = FALSE,
	//                               showProgress = FALSE)
	// ```
	static void ComputeAntTrajectoriesFunctor(const Experiment & experiment,
	                                          std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
	                                          const Time & start,
	                                          const Time & end,
	                                          Duration maximumGap,
	                                          const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                          bool computeZones = false,
	                                          bool singleThread = false);



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
	// @singleThread run this query on a single thread
	//
	// Computes trajectories for <Ant>. Those will be reported ordered
	// by ending time
	//
	// R version:
	// ```R
	// fmQueryComputeAntTrajectories(experiment,
	//                               start = NULL,
	//                               end = NULL,
	//                               maximuGap = fmSecond(1),
	//                               matcher = NULL,
	//                               computeZones = FALSE
	//                               singleThreaded = FALSE,
	//                               showProgress = FALSE)
	// ```
	static void ComputeAntTrajectories(const Experiment & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   const Time & start,
	                                   const Time & end,
	                                   Duration maximumGap,
	                                   const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                   bool computeZones = false,
	                                   bool singleThread = false);


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
	// @singleThread run this query on a single thread
	//
	// Computes interactions for <Ant>. Those will be reported ordered
	// by ending time. This version aimed to be used by language bindings to
	// avoid large data copy.
	//
	// R version:
	// ```R
	// fmQueryComputeAntInteractions(experiment,
	//                               start = NULL,
	//                               end = NULL,
	//                               maximuGap = fmSecond(1),
	//                               matcher = NULL,
	//                               singleThreaded = FALSE,
	//                               showProgress = FALSE,
	//                               reportTrajectories = FALSE)
	// ```
	static void ComputeAntInteractionsFunctor(const Experiment & experiment,
	                                          std::function<void ( const AntTrajectory::ConstPtr&)> storeTrajectory,
	                                          std::function<void ( const AntInteraction::ConstPtr&)> storeInteraction,
	                                          const Time & start,
	                                          const Time & end,
	                                          Duration maximumGap,
	                                          const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                          bool singleThread = false);



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
	// @singleThread run this query on a single thread
	//
	// Computes interactions for <Ant>. Those will be reported ordered
	// by ending time.
	//
	// R version:
	// ```R
	// fmQueryComputeAntInteractions(experiment,
	//                               start = NULL,
	//                               end = NULL,
	//                               maximuGap = fmSecond(1),
	//                               matcher = NULL,
	//                               singleThreaded = FALSE,
	//                               showProgress = FALSE,
	//                               reportTrajectories = FALSE)
	// ```
	static void ComputeAntInteractions(const Experiment & experiment,
	                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
	                                   std::vector<AntInteraction::ConstPtr> & interactions,
	                                   const Time & start,
	                                   const Time & end,
	                                   Duration maximumGap,
	                                   const Matcher::Ptr & matcher = Matcher::Ptr(),
	                                   bool singleThread = false);


};


} // namespace myrmidon
} // namespace fort
