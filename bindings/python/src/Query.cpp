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


py::list QueryComputeAntTrajectories(const fort::myrmidon::Experiment & experiment,
                                     fort::Time start,
                                     fort::Time end,
                                     fort::Duration maximumGap,
                                     const fort::myrmidon::Matcher::Ptr & matcher,
                                     bool computeZones,
                                     bool singleThreaded) {

	py::list res;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(experiment,
	                                                     [&res](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     res.append(t);
	                                                     },
	                                                     args);
	return res;
}

py::tuple QueryComputeAntInteractions(const fort::myrmidon::Experiment & experiment,
                                     fort::Time start,
                                     fort::Time end,
                                     fort::Duration maximumGap,
                                     const fort::myrmidon::Matcher::Ptr & matcher,
                                     bool reportFullTrajectories,
                                     bool singleThreaded) {

	py::list trajectories;
	py::list interactions;

	fort::myrmidon::Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ReportFullTrajectories = reportFullTrajectories;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::ComputeAntInteractionsFunctor(experiment,
	                                                     [&trajectories](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     trajectories.append(t);
	                                                     },
	                                                     [&interactions](const fort::myrmidon::AntInteraction::Ptr & i) {
		                                                     interactions.append(i);
	                                                     },
	                                                     args);
	return py::make_tuple(trajectories,interactions);

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
		.def_static("ComputeAntTrajectories",
		            &QueryComputeAntTrajectories,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = trajectoryArgs.Start,
		            "end"_a = trajectoryArgs.End,
		            "maximumGap"_a = trajectoryArgs.MaximumGap,
		            "matcher"_a = trajectoryArgs.Matcher,
		            "computeZones"_a = trajectoryArgs.ComputeZones,
		            "singleThreaded"_a = trajectoryArgs.SingleThreaded)
		.def_static("ComputeAntInteractions",
		            &QueryComputeAntInteractions,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = trajectoryArgs.Start,
		            "end"_a = trajectoryArgs.End,
		            "maximumGap"_a = trajectoryArgs.MaximumGap,
		            "matcher"_a = trajectoryArgs.Matcher,
		            "reportFullTrajectories"_a = trajectoryArgs.ComputeZones,
		            "singleThreaded"_a = trajectoryArgs.SingleThreaded)
		;


}
