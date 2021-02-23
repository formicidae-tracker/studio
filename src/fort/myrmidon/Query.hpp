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


	struct QueryArgs {
		// Start of the <Time> range (default: <Time::SinceEver>)
		Time Start;
		// End of the <Time> range (default: <Time::Forever>)
		Time End;
		// Uses a single thread for computation (default: false)
		bool SingleThreaded;
		// Forces result allocation from the calling thread (default: false)
		bool AllocationInCurrentThread;

		// Builds default arguments
		QueryArgs();
	};



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

	// Arguments for IdentifyFrames
	//
	// Arguments for <IdentifyFrames> and <IdentifyFramesFunctor>.
	struct IdentifyFramesArgs : public QueryArgs {
		bool ComputeZones;

		// Builds default arguments
		IdentifyFramesArgs();
	};

	// Identifies ants in frames - functor version
	// @experiment the <Experiment> to query for
	// @storeData a functor to store/convert the data
	// @args the <IdentifyFrameArgs> to use for this query
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
	                                  const IdentifyFramesArgs & params = IdentifyFramesArgs());


	// Identifies ants in frames
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame>
	// @args the <IdentifyFrameArgs> to use for this query
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
	                           const IdentifyFramesArgs & args = IdentifyFramesArgs() );


	// Finds <Collision> in data frame - functor version
	// @experiment the <Experiment> to query for
	// @storeData a functor to store the data as it is produced
	// @args the <CollideFramesArgs> to use for this query
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
	                                 const QueryArgs & args = QueryArgs());


	// Finds <Collision> in data frame
	// @experiment the <Experiment> to query for
	// @result the resulting <IdentifiedFrame> and <CollisionFrame>
	// @args the <CollideFramesArgs> to use for this query
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
	                          const QueryArgs & args = QueryArgs());


	// Arguments for ComputeAntTrajectories
	//
	// Arguments for <ComputeAntTrajectories> and
	// <ComputeAntTrajectoriesFunctor>.
	struct ComputeAntTrajectoriesArgs : public QueryArgs {
		// Maximum duration before considering the trajectory be two
		// different parts (default: 1s)
		Duration MaximumGap;
		// <Matcher> to reduce the query to an Ant subset (default: to
		// nullptr, i.e. anything).
		myrmidon::Matcher::Ptr Matcher;
		// Computes the zone of each Ant (default: false)
		bool ComputeZones;

		// Builds default arguments
		ComputeAntTrajectoriesArgs();
	};


	// Computes trajectories for ants - functor version
	// @experiment the <Experiment> to query for
	// @storeTrajectory a functor to store/covert the data
	// @args the <ComputeAntTrajectoriesArgs> to use for this query
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
	                                          const ComputeAntTrajectoriesArgs & args = ComputeAntTrajectoriesArgs());



	// Computes trajectories for ants
	// @experiment the <Experiment> to query for
	// @trajectories the resulting <IdentifiedFrame> and <CollisionFrame>
	// @args the <ComputeAntTrajectoriesArgs> to use for this query
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
	                                   const ComputeAntTrajectoriesArgs & args = ComputeAntTrajectoriesArgs());

	// Arguments for ComputeAntInteractions
	//
	// Arguments for <ComputeAntInteractions> and
	// <ComputeAntInteractionsFunctor>.
	struct ComputeAntInteractionsArgs : public QueryArgs{
		// Maximum duration before considering the trajectory be two
		// different parts (default: 1s)
		Duration MaximumGap;
		// <Matcher> to reduce the query to an Ant subset (default: to
		// nullptr, i.e. anything).
		myrmidon::Matcher::Ptr Matcher;

		// Reports full trajectories. If false only mean trajectory
		// during interactions will be reported, otherwise trajectory
		// will be computed like <ComputeAntTrajectories> and
		// Interaction points to sub-segment (default: true).
		bool ReportFullTrajectories;

		// Builds default arguments
		ComputeAntInteractionsArgs();
	};


	// Computes interactions for ants - functor version
	// @experiment the <Experiment> to query for
	// @storeTrajectory a functor to store/convert trajectories
	// @storeInteraction  a functor to store/convert interaction
	// @args the ComputeAntInteractionsArgs
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
	                                          const ComputeAntInteractionsArgs & args = ComputeAntInteractionsArgs());



	// Computes interactions for ants
	// @experiment the <Experiment> to query for
	// @trajectories the resulting <AntTrajectory>
	// @interactions the resulting <AntInteraction>
	// @args the ComputeAntInteractionsArgs
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
	                                   const ComputeAntInteractionsArgs & args = ComputeAntInteractionsArgs());


};


} // namespace myrmidon
} // namespace fort
