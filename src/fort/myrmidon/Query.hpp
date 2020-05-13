#pragma once

#include "Types.hpp"

#include "Experiment.hpp"



namespace fort {
namespace myrmidon {


// Wrapper for Queries on Experiment
//
// This class is a wrapper for all data queries that can be made on an
// Experiment. They takes advantages of multithreading to have
// efficient computation time.
class Query {
	// Computes all measurement for ant Ant
	// @experiment the <Experiment> to query for
	// @antID the desired <Ant>
	// @mTypeID the desired measurement type
	//
	// @return a <ComputedMeasurement::List> of the Measurement for the <Ant>
	static ComputedMeasurement::List ComputeMeasurementFor(const Experiment::ConstPtr & experiment,
	                                                       Ant::ID antID,
	                                                       MeasurementTypeID mTypeID);

	static TagStatistics::ByTagID ComputeTagStatistics(const Experiment::ConstPtr & experiment);


};


} // namespace myrmidon
} // namespace fort
