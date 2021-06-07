#include <pybind11/pybind11.h>

#include <fort/myrmidon/Query.hpp>

namespace py = pybind11;

py::list QueryIdentifyFrames(const fort::myrmidon::Experiment & experiment,
                             fort::Time start,
                             fort::Time end,
                             bool singleThreaded,
                             bool computeZones) {
	py::list res;
	fort::myrmidon::Query::IdentifyFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.ComputeZones = computeZones;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::IdentifyFramesFunctor(experiment,
	                                             [&res](const fort::myrmidon::IdentifiedFrame::Ptr & f) {
		                                             res.append(f);
	                                             },
	                                             args);
	return res;
}

py::list QueryCollideFrames(const fort::myrmidon::Experiment & experiment,
                            fort::Time start,
                            fort::Time end,
                            bool singleThreaded) {
	py::list res;
	fort::myrmidon::Query::QueryArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::CollideFramesFunctor(experiment,
	                                            [&res](const fort::myrmidon::Query::CollisionData & d) {
		                                             res.append(d);
	                                             },
	                                             args);
	return res;
}


void BindQuery(py::module_ & m) {
	using namespace pybind11::literals;

	fort::myrmidon::Query::IdentifyFramesArgs identifyArgs;
	fort::myrmidon::Query::QueryArgs collideArgs;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs trajectoryArgs;
	fort::myrmidon::Query::ComputeAntInteractionsArgs interactionArgs;

	py::class_<fort::myrmidon::Query>(m,"Query")
		.def_static("ComputeMeasurementFor",
		            &fort::myrmidon::Query::ComputeMeasurementFor,
		            "experiment"_a,
		            py::kw_only(),
		            "antID"_a,
		            "measurementTypeID"_a)
		.def_static("ComputeTagStatistics",
		            &fort::myrmidon::Query::ComputeTagStatistics,
		            "experiment"_a)
		.def_static("IdentifyFrame",&QueryIdentifyFrames,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = identifyArgs.Start,
		            "end"_a = identifyArgs.End,
		            "singleThreaded"_a = identifyArgs.SingleThreaded,
		            "computeZones"_a = identifyArgs.ComputeZones)
		.def_static("CollideFramesFunctor",
		            &QueryCollideFrames,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = collideArgs.Start,
		            "end"_a = collideArgs.End,
		            "singleThreaded"_a = collideArgs.SingleThreaded)


		;


}
