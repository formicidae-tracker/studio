#include "Query.hpp"

#include "priv/Experiment.hpp"

namespace fort {
namespace myrmidon {

ComputedMeasurement::List Query::ComputeMeasurementFor(const Experiment::ConstPtr & experiment,
                                                       Ant::ID antID,
                                                       MeasurementTypeID mTypeID) {
	ComputedMeasurement::List res;
	experiment->d_p->ComputeMeasurementsForAnt(res,antID,mTypeID);
	return res;
}

} // namespace myrmidon
} // namespace fort
