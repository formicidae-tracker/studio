#include "Query.hpp"

#include <thread>

#include <tbb/parallel_for.h>
#include <tbb/pipeline.h>

#include "TagStatistics.hpp"
#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"
#include "RawFrame.hpp"
#include "CollisionSolver.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

void Query::ComputeTagStatistics(const Experiment::ConstPtr & experiment,TagStatistics::ByTagID & result) {
	std::vector<TagStatistics::ByTagID> allSpaceResult;

	typedef std::vector<TagStatisticsHelper::Loader> StatisticLoaderList;
	for ( const auto & [spaceID,space] : experiment->CSpaces() ) {

		StatisticLoaderList loaders;
		for ( const auto & tdd : space->TrackingDataDirectories() ) {
			auto tddLoaders = tdd->StatisticsLoader();
			loaders.reserve(loaders.size() + tddLoaders.size());
			loaders.insert(loaders.end(),tddLoaders.begin(), tddLoaders.end());
		}
		std::vector<TagStatisticsHelper::Timed> spaceResults(loaders.size());

		tbb::parallel_for(tbb::blocked_range<size_t>(0,loaders.size()),
		                  [&loaders,&spaceResults](const tbb::blocked_range<size_t> & range) {
			                  for ( size_t idx = range.begin();
			                        idx != range.end();
			                        ++idx ) {
				                  spaceResults[idx] = loaders[idx]();
			                  }
		                  });

		allSpaceResult.push_back(TagStatisticsHelper::MergeTimed(spaceResults.begin(),spaceResults.end()).TagStats);
	}

	result = TagStatisticsHelper::MergeSpaced(allSpaceResult.begin(),allSpaceResult.end());
}

void Query::BuildRange(const Experiment::ConstPtr & experiment,
                       const Time::ConstPtr & start,
                       const Time::ConstPtr & end,
                       DataRangeBySpaceID & ranges) {

	const auto & spaces = experiment->CSpaces();
	for ( const auto & [spaceID,space] : spaces ) {
		for ( const auto & tdd : space->TrackingDataDirectories() ) {
			TrackingDataDirectory::const_iterator ibegin(tdd->begin()),iend(tdd->end());
			if ( !start == false ) {
				if (start->Before(tdd->StartDate()) == true) {
					continue;
				}
				ibegin = tdd->FrameAfter(*start);
			}
			if (!end == false ) {
				if (end->Before(tdd->StartDate()) == true ) {
					continue;
				}
				iend = tdd->FrameAfter(*end);
			}
			ranges[spaceID].push_back(std::make_pair(ibegin,iend));
		}
	}
}


Query::DataLoader::DataLoader(const DataRangeBySpaceID & dataRanges)
	: d_dataRanges(dataRanges)
	, d_rangeIterators(std::make_shared<RangesIteratorByID>())
	, d_dataIterators(std::make_shared<DataIteratorByID>()) {
	for ( const auto & [spaceID,ranges] : d_dataRanges ) {
		d_rangeIterators->insert(std::make_pair(spaceID,ranges.begin()));
		d_dataIterators->insert(std::make_pair(spaceID,std::move(ranges.begin()->first)));
	}
}


Query::RawData Query::DataLoader::operator()(tbb::flow_control & fc) const {
	auto res = (*this)();
	if (std::get<0>(res) == 0 ) {
		fc.stop();
	}
	return res;
}

Query::RawData Query::DataLoader::operator()() const {
	Space::ID next(0);
	Time nextTime;
	for (  auto & [spaceID,dataIter] : *d_dataIterators ) {
		auto & rangeIterator = d_rangeIterators->at(spaceID);
		if ( rangeIterator == d_dataRanges.at(spaceID).cend() ) {
			continue;
		}
		if ( dataIter == d_rangeIterators->at(spaceID)->second ) {
			++rangeIterator;
			if ( rangeIterator == d_dataRanges.at(spaceID).cend() ) {
				continue;
			}
			auto newDataIter = std::move(rangeIterator->first);
			dataIter = std::move(newDataIter);
		}
		const auto & dataTime = (*dataIter)->Frame().Time();

		if ( next == 0 || dataTime.Before(nextTime) ) {
			nextTime = dataTime;
			next = spaceID;
		}
	}

	if ( next == 0 ) {
		return Query::RawData(0,RawFrame::ConstPtr());
	}

	auto & dataIter = d_dataIterators->at(next);

	auto res = *(dataIter);
	++dataIter;
	return std::make_pair(next,res);
}


inline bool MonoIDMismatch(const Time & a,
                    const Time & b) {
	if ( a.HasMono() ) {
		if ( b.HasMono() ) {
			return a.MonoID() != b.MonoID();
		}
		return true;
	}
	return b.HasMono() == false;
}

Query::BuildingTrajectory::BuildingTrajectory(const IdentifiedFrame::ConstPtr & frame,
                                              const PositionedAnt & ant,
                                              const ZoneID * zone)
	: Trajectory(std::make_shared<AntTrajectory>())
	, Last(frame->FrameTime)
	, DataPoints({ant.Position.x(),ant.Position.y(),ant.Angle})
	, Durations({0.0}) {
	Trajectory->Ant = ant.ID;
	Trajectory->Start = frame->FrameTime;
	Trajectory->Space = frame->Space;

	if ( zone != nullptr ) {
		Zones.push_back(*zone);
	}


}


void Query::BuildingTrajectory::Append(const IdentifiedFrame::ConstPtr & frame,
                                       const PositionedAnt & ant,
                                       const ZoneID * zone) {
	Last = frame->FrameTime;
	Durations.push_back(frame->FrameTime.Sub(Trajectory->Start).Seconds());
	DataPoints.insert(DataPoints.end(),
	                  {ant.Position.x(),ant.Position.y(),ant.Angle});
	if ( zone != nullptr ) {
		Zones.push_back(*zone);
	}
}

AntTrajectory::ConstPtr Query::BuildingTrajectory::Terminate() const {
	if ( Durations.size() < 2 ) {
		return AntTrajectory::ConstPtr();
	}
	size_t nPoints = Durations.size();
	Trajectory->Positions.resize(nPoints,4);
	Trajectory->Positions.block(0,1,nPoints,3) = Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&DataPoints[0],nPoints,3);
	Trajectory->Positions.block(0,0,nPoints,1) = Eigen::Map<const Eigen::VectorXd>(&Durations[0],nPoints);
	Trajectory->Zones = Zones;
	return Trajectory;
}

Query::BuildingInteraction::BuildingInteraction(const Collision & collision,
                                                const Time & curTime)
	: IDs(collision.IDs)
	, Start(curTime)
	, Last(curTime) {
	for ( size_t i = 0; i < collision.Types.rows(); ++i ) {
		Types.insert(std::make_pair(collision.Types(i,0),
		                            collision.Types(i,1)));
	}
}

void Query::BuildingInteraction::Append(const Collision & collision,
                                        const Time & curTime) {
	Last = curTime;
	for ( size_t i = 0; i < collision.Types.rows(); ++i ) {
		Types.insert(std::make_pair(collision.Types(i,0),
		                            collision.Types(i,1)));
	}
}

AntInteraction::ConstPtr Query::BuildingInteraction::Terminate(const BuildingTrajectory & a,
                                                               const BuildingTrajectory & b ) const {
	if (Start == Last ) {
		return AntInteraction::ConstPtr();
	}
	auto res = std::make_shared<AntInteraction>();
	res->IDs = IDs;
	res->Space = a.Trajectory->Space;
	res->Types = InteractionTypes(Types.size(),2);
	size_t i = 0;
	for ( const auto & type : Types ) {
		res->Types(i,0) = type.first;
		res->Types(i,1) = type.second;
		++i;
	}
	auto findTrajectorySubSegment
		= [this](const BuildingTrajectory & t) {
			  double startTime = Start.Sub(t.Trajectory->Start).Seconds();
			  auto startIter = t.Durations.cbegin();
			  for ( ; startIter != t.Durations.cend(); ++startIter ) {
				  if ( *startIter >= startTime ) {
					  break;
				  }
			  }
			  AntTrajectorySegment res
				  = {
				     .Trajectory = t.Trajectory,
				     .Begin = size_t(startIter - t.Durations.cbegin()),
				     .End = t.Durations.size(),
			  };
			  return res;
		  };
	res->Trajectories.first = findTrajectorySubSegment(a);
	res->Trajectories.second = findTrajectorySubSegment(b);
	res->Start = Start;
	res->End = Last;
	return res;
}

std::function<void(const IdentifiedFrame::ConstPtr &)>
Query::BuildTrajectories(std::function<void(const AntTrajectory::ConstPtr &)> storeResult,
                         BuildingTrajectoryData & building,
                         Duration maxGap,
                         const Matcher::Ptr & matcher) {
	return [storeResult,
	        &building,
	        &matcher,
	        maxGap]( const IdentifiedFrame::ConstPtr & data ) {
		       if ( matcher ) {
			       matcher->SetUp(data,CollisionFrame::ConstPtr());
		       }
		       size_t i = 0;
		       for ( const auto & pa : data->Positions ) {
			       const ZoneID * zone = nullptr;
			       if ( data->Zones.size() != 0 ) {
				       zone = &(data->Zones[i]);
			       }
			       ++i;

			       auto & curTime = data->FrameTime;
			       if ( matcher && matcher->Match(pa.ID,0,{}) == false ) {
				       continue;
			       }

			       auto fi = building.find(pa.ID);
			       if ( fi != building.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last) == true
				            || curTime.Sub(fi->second.Last) > maxGap
				            || data->Space != fi->second.Trajectory->Space) {
					       auto res = fi->second.Terminate();
					       if ( res ) {
						       storeResult(res);
					       }
					       building.erase(fi);
					       fi = building.end();
				       } else {
					       fi->second.Append(data,pa,zone);
				       }
			       }

			       if ( fi == building.end() ) {
				       building.insert(std::make_pair(pa.ID,BuildingTrajectory(data,pa,zone)));;
			       }
		       }
	       };
}





std::function<void(const Query::CollisionData &)>
Query::BuildInteractions(std::function<void(const AntTrajectory::ConstPtr&)> storeTrajectory,
                         std::function<void(const AntInteraction::ConstPtr&)> storeInteraction,
                         BuildingTrajectoryData & currentTrajectories,
                         BuildingInteractionData & currentInteractions,
                         Duration maxGap,
                         const Matcher::Ptr & matcher) {
	return [storeTrajectory,
	        storeInteraction,
	        &currentTrajectories,
	        &currentInteractions,
	        &matcher,
	        maxGap]( const CollisionData & data ) {
		       if ( matcher ) {
			       matcher->SetUp(std::get<0>(data),std::get<1>(data));
		       }

		       std::vector<std::pair<PositionedAnt,const ZoneID*>> toTerminate;

		       auto & curTime = std::get<0>(data)->FrameTime;

		       size_t i = 0;
		       for (  const auto & pa : std::get<0>(data)->Positions ) {
			       const ZoneID * zone = nullptr;
			       if ( std::get<0>(data)->Zones.size() != 0 ) {
				       zone = &(std::get<0>(data)->Zones[i]);
			       }
			       ++i;

			       if ( matcher && matcher->Match(pa.ID,0,{}) == false ) {
				       continue;
			       }

			       auto fi = currentTrajectories.find(pa.ID);
			       if ( fi != currentTrajectories.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last)
				            || curTime.Sub(fi->second.Last) > maxGap
				            || std::get<0>(data)->Space != fi->second.Trajectory->Space) {
					       std::vector<InteractionID> toRemove;
					       for ( const auto & [IDs,interaction] : currentInteractions ) {
						       if ( IDs.first != pa.ID && IDs.second != pa.ID ) {
							       continue;
						       }
						       toRemove.push_back(IDs);
						       try {
							       auto toStore = interaction.Terminate(currentTrajectories.at(IDs.first),
							                                            currentTrajectories.at(IDs.second));
							       if ( toStore ) {
								       storeInteraction(toStore);
							       }
						       } catch ( const std::exception & ) {
						       }
					       }
					       toTerminate.push_back(std::make_pair(pa,zone));
					       for ( const auto & IDs : toRemove ) {
						       currentInteractions.erase(IDs);
					       }
				       } else {
					       fi->second.Append(std::get<0>(data),pa,zone);
				       }
			       } else {
				       currentTrajectories.insert(std::make_pair(pa.ID,BuildingTrajectory(std::get<0>(data),pa,zone)));;
			       }
		       }

		       for ( const auto & [pa,zone] : toTerminate ) {
			       auto & curTraj = currentTrajectories.at(pa.ID);
			       auto toStore = curTraj.Terminate();
			       if ( toStore ) {
				       storeTrajectory(toStore);
			       }
			       curTraj = BuildingTrajectory(std::get<0>(data),pa,zone);
		       }


		       for ( const auto & pInter : std::get<1>(data)->Collisions ) {
			       if (matcher && matcher->Match(pInter.IDs.first,
			                                     pInter.IDs.second,
			                                     pInter.Types) == false ) {
				       continue;
			       }

			       auto fi = currentInteractions.find(pInter.IDs);
			       static size_t here(0);
			       if ( fi != currentInteractions.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last) == true
				            || curTime.Sub(fi->second.Last) > maxGap ) {
					       try {
						       auto toStore = fi->second.Terminate(currentTrajectories.at(pInter.IDs.first),
						                                           currentTrajectories.at(pInter.IDs.second));
						       if ( toStore ) {
							       storeInteraction(toStore);
						       }
					       } catch ( const std::exception & )  {
					       }
					       currentInteractions.erase(fi);
					       fi = currentInteractions.end();
				       } else {
					       fi->second.Append(pInter,curTime);
				       }
			       }

			       if ( fi == currentInteractions.end() ) {
				       currentInteractions.insert(std::make_pair(pInter.IDs,BuildingInteraction(pInter,curTime)));
			       }

		       }

	       };

}

void Query::IdentifyFrames(const Experiment::ConstPtr & experiment,
                           std::function<void ( const IdentifiedFrame::ConstPtr &)> storeDataFunctor,
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
			storeDataFunctor(identified);
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
		          storeDataFunctor);

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);
}

void Query::CollideFrames(const Experiment::ConstPtr & experiment,
                          std::function<void (const CollisionData &)> storeDataFunctor,
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
			storeDataFunctor({identified,collided});
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
		          storeDataFunctor);

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);

}

void Query::ComputeTrajectories(const Experiment::ConstPtr & experiment,
                                std::function<void (const AntTrajectory::ConstPtr &)> storeDataFunctor,
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
		BuildTrajectories(storeDataFunctor,
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
			storeDataFunctor(res);
		}
	}
}

void Query::ComputeAntInteractions(const Experiment::ConstPtr & experiment,
                                   std::function<void ( const AntTrajectory::ConstPtr &) > storeTrajectory,
                                   std::function<void ( const AntInteraction::ConstPtr &) > storeInteraction,
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
		BuildInteractions(storeTrajectory,
		                  storeInteraction,
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
			storeTrajectory(res);
		}
	}

	for ( const auto & [IDs,bInteraction] : currentInteractions ) {
		auto res = bInteraction.Terminate(currentTrajectories.at(IDs.first),
		                                  currentTrajectories.at(IDs.second));
		if ( res ) {
			storeInteraction(res);
		}
	}
}





} // namespace priv
} // namespace myrmidon
} // namespace fort
