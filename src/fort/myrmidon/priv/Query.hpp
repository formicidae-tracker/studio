#pragma once

#include <fort/myrmidon/Types.hpp>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Query {
public:

	static void ComputeTagStatistics(const Experiment::ConstPtr & experiment,
	                                 TagStatistics::ByTagID & result);

	static void IdentifyFrames(const Experiment::ConstPtr & experiment,
	                           const Time::ConstPtr & start,
	                           const Time::ConstPtr & end,
	                           std::vector<IdentifiedFrame> & result);

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
