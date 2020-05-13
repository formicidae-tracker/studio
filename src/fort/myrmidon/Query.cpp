#include "Query.hpp"

#include <tbb/pipeline.h>

#include "priv/Query.hpp"

namespace fort {
namespace myrmidon {

ComputedMeasurement::List Query::ComputeMeasurementFor(const Experiment::ConstPtr & experiment,
                                                       Ant::ID antID,
                                                       MeasurementTypeID mTypeID) {
	ComputedMeasurement::List res;
	experiment->d_p->ComputeMeasurementsForAnt(res,antID,mTypeID);
	return res;
}

TagStatistics::ByTagID Query::ComputeTagStatistics(const Experiment::ConstPtr & experiment) {
	TagStatistics::ByTagID res;
	priv::Query::ComputeTagStatistics(experiment->d_p,res);
	return res;
}

} // namespace myrmidon
} // namespace fort
