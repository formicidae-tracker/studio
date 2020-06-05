#include "Query.hpp"

#include <tbb/pipeline.h>

#include "priv/Query.hpp"



namespace fort {
namespace myrmidon {


template <typename T>
class WrapFunctor {
public:
	typedef std::function<void(const T &)> StoreFunction;
	WrapFunctor(const StoreFunction & functor)
		: d_functor(functor) {
	}
	inline void operator=(const T & t) const {
		d_functor(t);
	}
private:
	StoreFunction d_functor;
};


ComputedMeasurement::List Query::ComputeMeasurementFor(const CExperiment & experiment,
                                                       Ant::ID antID,
                                                       MeasurementTypeID mTypeID) {
	ComputedMeasurement::List res;
	experiment.d_p->ComputeMeasurementsForAnt(res,antID,mTypeID);
	return res;
}

TagStatistics::ByTagID Query::ComputeTagStatistics(const CExperiment & experiment) {
	TagStatistics::ByTagID res;
	priv::Query::ComputeTagStatistics(experiment.d_p,res);
	return res;
}


void Query::IdentifyFramesFunctor(const CExperiment & experiment,
                                  std::function<void (const IdentifiedFrame::ConstPtr &)> storeData,
                                  const Time::ConstPtr & start,
                                  const Time::ConstPtr & end,
                                  bool computeZones,
                                  bool singleThreaded) {
	WrapFunctor inserter(storeData);
	priv::Query::IdentifyFrames(experiment.d_p,inserter,start,end,computeZones,singleThreaded);
}


void Query::IdentifyFrames(const CExperiment & experiment,
                           std::vector<IdentifiedFrame::ConstPtr> & result,
                           const Time::ConstPtr & start,
                           const Time::ConstPtr & end,
                           bool computeZones,
                           bool singleThread) {
	auto backInserter = std::back_inserter(result);
	priv::Query::IdentifyFrames(experiment.d_p,backInserter,start,end,computeZones,singleThread);
}


void Query::CollideFramesFunctor(const CExperiment & experiment,
                                 std::function<void (const CollisionData & data)> storeData,
                                 const Time::ConstPtr & start,
                                 const Time::ConstPtr & end,
                                 bool singleThread) {
	WrapFunctor backInserter(storeData);
	priv::Query::CollideFrames(experiment.d_p,backInserter,start,end,singleThread);
}


void Query::CollideFrames(const CExperiment & experiment,
                          std::vector<CollisionData> & result,
                          const Time::ConstPtr & start,
                          const Time::ConstPtr & end,
                          bool singleThread) {
	auto backInserter = std::back_inserter(result);
	priv::Query::CollideFrames(experiment.d_p,backInserter,start,end,singleThread);
}

void Query::ComputeAntTrajectoriesFunctor(const CExperiment & experiment,
                                          std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectory,
                                          const Time::ConstPtr & start,
                                          const Time::ConstPtr & end,
                                          Duration maximumGap,
                                          const Matcher::Ptr & matcher,
                                          bool computeZones,
                                          bool singleThread) {
	WrapFunctor backInserter(storeTrajectory);
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 backInserter,
	                                 start,
	                                 end,
	                                 maximumGap,
	                                 !matcher ? Matcher::PPtr() : matcher->d_p,
	                                 computeZones,
	                                 singleThread);
}



void Query::ComputeAntTrajectories(const CExperiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end,
                                   Duration maximumGap,
                                   const Matcher::Ptr & matcher,
                                   bool computeZones,
                                   bool singleThread) {
	auto backInserter = std::back_inserter(trajectories);
	priv::Query::ComputeTrajectories(experiment.d_p,
	                                 backInserter,
	                                 start,
	                                 end,
	                                 maximumGap,
	                                 !matcher ? Matcher::PPtr() : matcher->d_p,
	                                 computeZones,
	                                 singleThread);
}

void Query::ComputeAntInteractionsFunctor(const CExperiment & experiment,
                                          std::function<void ( const AntTrajectory::ConstPtr&)> storeTrajectory,
                                          std::function<void ( const AntInteraction::ConstPtr&)> storeInteraction,
                                          const Time::ConstPtr & start,
                                          const Time::ConstPtr & end,
                                          Duration maximumGap,
                                          const Matcher::Ptr & matcher,
                                          bool singleThread) {
	WrapFunctor trajInserter(storeTrajectory);
	WrapFunctor interInserter(storeInteraction);

	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    trajInserter,
	                                    interInserter,
	                                    start,
	                                    end,
	                                    maximumGap,
	                                    !matcher ? Matcher::PPtr() : matcher->d_p,
	                                    singleThread);
}


void Query::ComputeAntInteractions(const CExperiment & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   std::vector<AntInteraction::ConstPtr> & interactions,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end,
                                   Duration maximumGap,
                                   const Matcher::Ptr & matcher,
                                   bool singleThread) {
	auto trajInserter = std::back_inserter(trajectories);
	auto interInserter = std::back_inserter(interactions);

	priv::Query::ComputeAntInteractions(experiment.d_p,
	                                    trajInserter,
	                                    interInserter,
	                                    start,
	                                    end,
	                                    maximumGap,
	                                    !matcher ? Matcher::PPtr() : matcher->d_p,
	                                    singleThread);
}


} // namespace myrmidon
} // namespace fort
