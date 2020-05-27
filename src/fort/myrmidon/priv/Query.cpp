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

Query::RawData Query::DataLoader::operator()( tbb::flow_control & fc) const {
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
		fc.stop();
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
	: Ant(ant.ID)
	, SpaceID(frame->Space)
	, Start(frame->FrameTime)
	, Last(frame->FrameTime)
	, DataPoints({ant.Position.x(),ant.Position.y(),ant.Angle})
	, Durations({0}) {
	if ( zone != nullptr ) {
		Zones.push_back(*zone);
	}
}


void Query::BuildingTrajectory::Append(const IdentifiedFrame::ConstPtr & frame,
                                       const PositionedAnt & ant,
                                       const ZoneID * zone) {
	Last = frame->FrameTime;
	Durations.push_back(frame->FrameTime.Sub(Start).Nanoseconds());
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
	auto res = std::make_shared<AntTrajectory>();
	res->Space = SpaceID;
	res->Ant = Ant;
	res->Start = Start;
	res->Positions = Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&DataPoints[0],DataPoints.size()/3,3);
	res->Nanoseconds = Durations;
	res->Zones = Zones;
	return res;
}

Query::BuildingInteraction::BuildingInteraction(const Collision & collision,
                                                const Time & curTime)
	: IDs(collision.IDs)
	, Start(curTime)
	, Last(curTime) {
	for ( const auto & type : collision.InteractionTypes ) {
		Types.insert(type);
	}
}

void Query::BuildingInteraction::Append(const Collision & collision,
                                        const Time & curTime) {
	Last = curTime;
	for ( const auto & type : collision.InteractionTypes ) {
		Types.insert(type);
	}
}

AntInteraction::ConstPtr Query::BuildingInteraction::Terminate(const BuildingTrajectory & a,
                                                               const BuildingTrajectory & b ) const {
	auto res = std::make_shared<AntInteraction>();
	res->IDs = IDs;
	for ( const auto & type : Types ) {
		res->Types.push_back(type);
	}
	auto cutTrajectory
		= [this](const BuildingTrajectory & t) {
			  auto res = std::const_pointer_cast<AntTrajectory>(t.Terminate());
			  uint64_t toTrim = Start.Sub(t.Start).Nanoseconds();
			  auto fi = std::find_if(res->Nanoseconds.begin(),
			                         res->Nanoseconds.end(),
			                         [toTrim](uint64_t d) {
				                         return d >= toTrim;
			                         });
			  size_t elems = fi - res->Nanoseconds.begin();
			  res->Start = Start;

			  res->Nanoseconds.erase(res->Nanoseconds.begin(),fi);
			  res->Positions = res->Positions.block(elems,0,res->Positions.rows()-elems,3);
			  return res;
		  };
	res->Trajectories.first = cutTrajectory(a);
	res->Trajectories.second = cutTrajectory(b);
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
				            || data->Space != fi->second.SpaceID) {
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
				            || std::get<0>(data)->Space != fi->second.SpaceID) {
					       std::vector<InteractionID> toRemove;
					       for ( const auto & [IDs,interaction] : currentInteractions ) {
						       if ( IDs.first != pa.ID && IDs.second != pa.ID ) {
							       continue;
						       }
						       toRemove.push_back(IDs);
						       try {
							       storeInteraction(interaction.Terminate(currentTrajectories.at(IDs.first),
							                                              currentTrajectories.at(IDs.second)));
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
			       storeTrajectory(curTraj.Terminate());
			       curTraj = BuildingTrajectory(std::get<0>(data),pa,zone);
		       }


		       for ( const auto & pInter : std::get<1>(data)->Collisions ) {
			       if (matcher && matcher->Match(pInter.IDs.first,
			                                     pInter.IDs.second,
			                                     pInter.InteractionTypes) == false ) {
				       continue;
			       }

			       auto fi = currentInteractions.find(pInter.IDs);
			       static size_t here(0);
			       if ( fi != currentInteractions.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last) == true
				            || curTime.Sub(fi->second.Last) > maxGap ) {
					       try {
					            storeInteraction(fi->second.Terminate(currentTrajectories.at(pInter.IDs.first),
					                                                  currentTrajectories.at(pInter.IDs.second)));
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


} // namespace priv
} // namespace myrmidon
} // namespace fort
