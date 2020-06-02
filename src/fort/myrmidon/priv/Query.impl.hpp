#pragma once

#include "Query.hpp"

#include "CollisionSolver.hpp"
#include "RawFrame.hpp"

#include <thread>

namespace fort {
namespace myrmidon {
namespace priv {

template <typename OutputIter>
inline
void Query::IdentifyFrames(const Experiment::ConstPtr & experiment,
                           OutputIter & output,
                           const Time::ConstPtr & start,
                           const Time::ConstPtr & end,
                           bool computeZones,
                           bool singleThread) {
	auto identifier = experiment->CIdentifier().Compile();
	CollisionSolver::ConstPtr collider;
	if ( computeZones == true ) {
		collider = experiment->CompileCollisionSolver();
	}
	DataRangeBySpaceID ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}

	if (singleThread == true ) {
		DataLoader loader(ranges);
		for(;;) {
			auto raw = loader();
			if ( std::get<0>(raw) == 0 ) {
				break;
			}
			auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
			if ( collider ) {
				auto zoner = collider->ZonerFor(identified);
				identified->Zones.reserve(identified->Positions.size());
				for ( const auto & p : identified->Positions ) {
					identified->Zones.push_back(zoner->LocateAnt(p));
				}
			}
			output = identified;
		}
		return;
	}

	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,DataLoader(ranges));

	tbb::filter_t<RawData,IdentifiedFrame::ConstPtr>
		computeData(tbb::filter::parallel,
		            [identifier,collider](const RawData & rawData ) -> IdentifiedFrame::ConstPtr {
			            auto identified = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
			            if ( collider ) {
				            auto zoner = collider->ZonerFor(identified);
				            identified->Zones.reserve(identified->Positions.size());
				            for ( const auto & p : identified->Positions ) {
					            identified->Zones.push_back(zoner->LocateAnt(p));
				            }
			            }
			            return identified;
		            });


	tbb::filter_t<IdentifiedFrame::ConstPtr,void>
		storeData(tbb::filter::serial_in_order,
		          [&output](const IdentifiedFrame::ConstPtr & res) {
			          output = res;
		          });

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);

}

template <typename OutputIter>
inline
void Query::CollideFrames(const Experiment::ConstPtr & experiment,
                          OutputIter & output,
                          const Time::ConstPtr & start,
                          const Time::ConstPtr & end,
                          bool singleThreaded) {
	auto identifier = experiment->CIdentifier().Compile();
	auto solver = experiment->CompileCollisionSolver();
	DataRangeBySpaceID ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}

	if ( singleThreaded == true ) {
		DataLoader loader(ranges);
		for (;;) {
			auto raw = loader();
			if ( std::get<0>(raw) == 0 ) {
				break;
			}
			auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
			auto collided = solver->ComputeCollisions(identified);
			output = {identified,collided};
		}
		return;
	}

	tbb::filter_t<void,RawData >
		loadData(tbb::filter::serial_in_order,DataLoader(ranges));

	tbb::filter_t<RawData,
	              CollisionData>
		computeData(tbb::filter::parallel,
		            [identifier,solver](const RawData & rawData ) -> CollisionData {
			            auto identified = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
			            auto interacted = solver->ComputeCollisions(identified);
			            return std::make_pair(identified,interacted);
		            });


	tbb::filter_t<CollisionData,void>
		storeData(tbb::filter::serial_in_order,
		          [&output](const CollisionData & res) {
			          output = res;
		          });

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);

}

template <typename OutputIter>
inline
void Query::ComputeTrajectories(const Experiment::ConstPtr & experiment,
                                OutputIter & output,
                                const Time::ConstPtr & start,
                                const Time::ConstPtr & end,
                                Duration maximumGap,
                                const Matcher::Ptr & matcher,
                                bool computeZones,
                                bool singleThreaded) {
	auto identifier = experiment->CIdentifier().Compile();
	CollisionSolver::ConstPtr collider;
	if ( computeZones == true ) {
		collider = experiment->CompileCollisionSolver();
	}
	if ( matcher ) {
		matcher->SetUpOnce(experiment->CIdentifier().CAnts());
	}
	DataRangeBySpaceID ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}
	BuildingTrajectoryData currentTrajectories;
	auto computeTrajectoriesFunction =
		BuildTrajectories([&output](const AntTrajectory::ConstPtr & t) {
			                  output = t;
		                  },
			currentTrajectories,
			maximumGap,
			matcher);
	if ( singleThreaded == true ) {
		DataLoader loader(ranges);
		for (;;) {
			auto raw = loader();
			if ( std::get<0>(raw) == 0 ) {
				break;
			}
			auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
			if ( collider ) {
				auto zoner = collider->ZonerFor(identified);
				identified->Zones.reserve(identified->Positions.size());
				for ( const auto & p : identified->Positions ) {
					identified->Zones.push_back(zoner->LocateAnt(p));
				}
			}
			computeTrajectoriesFunction(identified);
		}
	} else {
		tbb::filter_t<void,RawData>
			loadData(tbb::filter::serial_in_order,DataLoader(ranges));

		std::string currentTddURI;
		tbb::filter_t<RawData,IdentifiedFrame::ConstPtr>
			computeData(tbb::filter::parallel,
			            [identifier,collider](const RawData & rawData ) -> IdentifiedFrame::ConstPtr {
				            auto identified = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
				            if ( collider ) {
					            auto zoner = collider->ZonerFor(identified);
					            identified->Zones.reserve(identified->Positions.size());
					            for ( const auto & p : identified->Positions ) {
						            identified->Zones.push_back(zoner->LocateAnt(p));
					            }
				            }
				            return identified;
			            });

		tbb::filter_t<IdentifiedFrame::ConstPtr,void>
			computeTrajectories(tbb::filter::serial_in_order,
			                    computeTrajectoriesFunction);

		tbb::parallel_pipeline(std::thread::hardware_concurrency() * 2,
		                       loadData & computeData & computeTrajectories);
	}

	for ( const auto & [antID,bTrajectory] : currentTrajectories ) {
		auto res = bTrajectory.Terminate();
		if ( res ) {
			output = res;
		}
	}
}

template <typename TrajectoryOutputIter,
          typename InteractionOutputIter>
inline
void Query::ComputeAntInteractions(const Experiment::ConstPtr & experiment,
                                   TrajectoryOutputIter & trajectoryOutput,
                                   InteractionOutputIter & interactionOutput,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end,
                                   Duration maximumGap,
                                   const Matcher::Ptr & matcher,
                                   bool singleThreaded) {

	auto identifier = experiment->CIdentifier().Compile();
	auto solver = experiment->CompileCollisionSolver();

	if ( matcher ) {
		matcher->SetUpOnce(experiment->CIdentifier().CAnts());
	}
	DataRangeBySpaceID ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}

	BuildingTrajectoryData currentTrajectories;
	BuildingInteractionData currentInteractions;
	auto buildInteractionsFunction =
		BuildInteractions([&trajectoryOutput](const AntTrajectory::ConstPtr & t) {
			                  trajectoryOutput = t;
		                  },
			[&interactionOutput](const AntInteraction::ConstPtr & i) {
				interactionOutput = i;
			},
			currentTrajectories,
			currentInteractions,
			maximumGap,
			matcher);

	if ( singleThreaded == true ) {
		DataLoader loader(ranges);
		for (;;) {
			auto raw = loader();
			if ( std::get<0>(raw) == 0 ) {
				break;
			}
			auto identified  = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
			auto collided = solver->ComputeCollisions(identified);
			buildInteractionsFunction({identified,collided});
		}
	} else {

		tbb::filter_t<void,RawData>
			loadData(tbb::filter::serial_in_order,DataLoader(ranges));

		tbb::filter_t<RawData,CollisionData>
			computeData(tbb::filter::parallel,
			            [identifier,solver](const RawData & rawData ) -> CollisionData {
				            auto identified  = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
				            auto interacted = solver->ComputeCollisions(identified);
				            return std::make_pair(identified,interacted);
			            });


		tbb::filter_t<CollisionData,void>
			computeInteractions(tbb::filter::serial_in_order,
			                    buildInteractionsFunction);

		tbb::parallel_pipeline(std::thread::hardware_concurrency() * 2,
		                       loadData & computeData & computeInteractions);
	}

	for ( const auto & [antID,bTrajectory] : currentTrajectories ) {
		auto res = bTrajectory.Terminate();
		if ( res ) {
			trajectoryOutput = res;
		}
	}

	for ( const auto & [IDs,bInteraction] : currentInteractions ) {
		auto res = bInteraction.Terminate(currentTrajectories.at(IDs.first),
		                                  currentTrajectories.at(IDs.second));
		if ( res ) {
			interactionOutput = res;
		}
	}
}




} // namespace priv
} // namespace myrmidon
} // namespace fort
