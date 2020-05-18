#include "Query.hpp"

#include <thread>

#include <tbb/parallel_for.h>
#include <tbb/pipeline.h>

#include "TagStatistics.hpp"
#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"
#include "RawFrame.hpp"
#include "InteractionSolver.hpp"

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
                       DataRangeWithSpace & ranges) {

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
			ranges.push_back(std::make_pair(spaceID,std::make_pair(ibegin,iend)));
		}
	}
}


std::function<Query::RawData(tbb::flow_control &fc)>
Query::LoadData(const DataRangeWithSpace & ranges,
                DataRangeWithSpace::iterator & rangeIter,
                TrackingDataDirectory::const_iterator & dataIter) {
	return [&ranges,
	        &rangeIter,
	        &dataIter](tbb::flow_control & fc) -> RawData {
		       if ( dataIter == rangeIter->second.second ) {
			       ++rangeIter;
			       if ( rangeIter == ranges.end() ) {
				       fc.stop();
				       return std::make_pair(0,RawFrame::ConstPtr());
			       }
			       dataIter = std::move(rangeIter->second.first);
		       }

		       auto res = *dataIter;
		       ++dataIter;
		       return std::make_pair(rangeIter->first,res);
	};
}

void Query::IdentifyFrames(const Experiment::ConstPtr & experiment,
                           std::vector<IdentifiedData> & result,
                           const Time::ConstPtr & start,
                           const Time::ConstPtr & end) {
	auto identifier = experiment->CIdentifier().Compile();
	DataRangeWithSpace ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}
	auto rangeIter = ranges.begin();
	auto dataIter = ranges.front().second.first;
	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial,LoadData(ranges,rangeIter,dataIter));

	tbb::filter_t<RawData,IdentifiedData>
		computeData(tbb::filter::parallel,
		            [identifier](const RawData & rawData ) {
			            return std::make_pair(std::get<0>(rawData),std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData)));
		            });


	tbb::filter_t<IdentifiedData,void>
		storeData(tbb::filter::serial,
		          [&result](const IdentifiedData & res) {
			          result.push_back(res);
		          });

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);

}

void Query::InteractFrame(const Experiment::ConstPtr & experiment,
                          std::vector<InteractionData> & result,
                          const Time::ConstPtr & start,
                          const Time::ConstPtr & end) {
	auto identifier = experiment->CIdentifier().Compile();
	auto solver = experiment->CompileInteractionSolver();
	DataRangeWithSpace ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}
	auto rangeIter = ranges.begin();
	auto dataIter = ranges.front().second.first;
	tbb::filter_t<void,RawData >
		loadData(tbb::filter::serial,LoadData(ranges,rangeIter,dataIter));

	tbb::filter_t<RawData,
	              InteractionData>
		computeData(tbb::filter::parallel,
		            [identifier,solver](const RawData & rawData ) {
			            auto identified = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
			            auto interacted = solver->ComputeInteractions(identified);
			            return std::make_tuple(std::get<0>(rawData),identified,interacted);
		            });


	tbb::filter_t<InteractionData,void>
		storeData(tbb::filter::serial,
		          [&result](const InteractionData & res) {
			          result.push_back(res);
		          });

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,loadData & computeData & storeData);

}


void Query::ComputeTrajectories(const Experiment::ConstPtr & experiment,
                                std::vector<AntTrajectory::ConstPtr> & trajectories,
                                const Time::ConstPtr & start,
                                const Time::ConstPtr & end,
                                Duration maximumGap,
                                Matcher::Ptr matcher) {
	auto identifier = experiment->CIdentifier().Compile();
	if ( matcher ) {
		matcher->SetUpOnce(experiment->CIdentifier().CAnts());
	}
	DataRangeWithSpace ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}
	auto rangeIter = ranges.begin();
	auto dataIter = ranges.front().second.first;
	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,LoadData(ranges,rangeIter,dataIter));

	std::string currentTddURI;
	tbb::filter_t<RawData,IdentifiedData>
		computeData(tbb::filter::parallel,
		            [identifier](const RawData & rawData ) {
			            return std::make_pair(std::get<0>(rawData),
			                                  std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData)));
		            });

	 BuildingTrajectoryData currentTrajectories;
	 tbb::filter_t<IdentifiedData,void>
		 computeTrajectories(tbb::filter::serial_in_order,
		                     BuildTrajectories(trajectories,currentTrajectories,maximumGap,matcher));

	tbb::parallel_pipeline(std::thread::hardware_concurrency() * 2,
	                       loadData & computeData & computeTrajectories);

	for ( const auto & [antID,bTrajectory] : currentTrajectories ) {
		auto res = bTrajectory.Terminate(antID);
		if ( res ) {
			trajectories.push_back(res);
		}
	}

	// sorts trajectories by Ant and Time, mixed Space
	std::sort(trajectories.begin(),
	          trajectories.end(),
	          [](const AntTrajectory::ConstPtr & a,
	             const AntTrajectory::ConstPtr & b) {
		          if ( a->Ant == b->Ant ) {
			          return a->Start < b->Start;
		          }
		          return a->Ant < b->Ant;
	          });
}


void Query::ComputeAntInteractions(const Experiment::ConstPtr & experiment,
                                   std::vector<AntTrajectory::ConstPtr> & trajectories,
                                   std::vector<AntInteraction::ConstPtr> & interactions,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end,
                                   Duration maximumGap,
                                   Matcher::Ptr matcher) {

	auto identifier = experiment->CIdentifier().Compile();
	auto solver = experiment->CompileInteractionSolver();

	if ( matcher ) {
		matcher->SetUpOnce(experiment->CIdentifier().CAnts());
	}
	DataRangeWithSpace ranges;
	BuildRange(experiment,start,end,ranges);
	if ( ranges.empty() ) {
		return;
	}
	auto rangeIter = ranges.begin();
	auto dataIter = ranges.front().second.first;
	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,LoadData(ranges,rangeIter,dataIter));

	tbb::filter_t<RawData,InteractionData>
		computeData(tbb::filter::parallel,
		            [identifier,solver](const RawData & rawData ) {
			            auto identified  = std::get<1>(rawData)->IdentifyFrom(*identifier,std::get<0>(rawData));
			            auto interacted = solver->ComputeInteractions(identified);
			            return std::make_tuple(std::get<0>(rawData),identified,interacted);
		            });

	 BuildingTrajectoryData currentTrajectories;
	 BuildingInteractionData currentInteractions;

	tbb::filter_t<InteractionData,void>
		computeInteractions(tbb::filter::serial_in_order,
		                    BuildInteractions(trajectories,
		                                      interactions,
		                                      currentTrajectories,
		                                      currentInteractions,
		                                      maximumGap,
		                                      matcher));

	tbb::parallel_pipeline(std::thread::hardware_concurrency() * 2,
	                       loadData & computeData & computeInteractions);

	for ( const auto & [antID,bTrajectory] : currentTrajectories ) {
		auto res = bTrajectory.Terminate(antID);
		if ( res ) {
			trajectories.push_back(res);
		}
	}

	for ( const auto & [IDs,bInteraction] : currentInteractions ) {
		auto res = bInteraction.Terminate(currentTrajectories.at(IDs.first),
		                                  currentTrajectories.at(IDs.second));
		if ( res ) {
			interactions.push_back(res);
		}
	}

	// sorts trajectories by Ant and Time, mixed Space
	std::sort(trajectories.begin(),
	          trajectories.end(),
	          [](const AntTrajectory::ConstPtr & a,
	             const AntTrajectory::ConstPtr & b) {
		          if ( a->Ant == b->Ant ) {
			          return a->Start < b->Start;
		          }
		          return a->Ant < b->Ant;
	          });

	// sorts interactions too.
	std::sort(interactions.begin(),
	          interactions.end(),
	          [](const AntInteraction::ConstPtr & a,
	             const AntInteraction::ConstPtr & b) {
		          if ( a->IDs == b->IDs ) {
			          return a->Start < b->Start;
		          }
		          return a->IDs < b->IDs;
	          });

}


AntTrajectory::ConstPtr Query::BuildingTrajectory::Terminate(AntID antID) const {
	if ( Durations.size() < 2 ) {
		return AntTrajectory::ConstPtr();
	}
	auto res = std::make_shared<AntTrajectory>();
	res->Space = SpaceID;
	res->Ant = antID;
	res->Start = Start;
	res->Positions = Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&DataPoints[0],DataPoints.size()/3,3);
	res->Nanoseconds = Durations;
	return res;
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

AntInteraction::ConstPtr Query::BuildingInteraction::Terminate(const BuildingTrajectory & a,
                                                               const BuildingTrajectory & b ) const {
	auto res = std::make_shared<AntInteraction>();
	res->IDs = IDs;
	for ( const auto & type : Types ) {
		res->Types.push_back(type);
	}
	auto cutTrajectory
		= [this](const BuildingTrajectory & t,
		         AntID antID) {
			  auto res = std::const_pointer_cast<AntTrajectory>(t.Terminate(antID));
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
	res->Trajectories.first = cutTrajectory(a,res->IDs.first);
	res->Trajectories.second = cutTrajectory(b,res->IDs.second);
	res->Start = Start;
	res->End = Last;
	return res;
}


std::function<void(const Query::IdentifiedData &)>
Query::BuildTrajectories(std::vector<AntTrajectory::ConstPtr> & result,
                         BuildingTrajectoryData & building,
                         Duration maxGap,
                         const Matcher::Ptr & matcher) {
	return [&result,
	        &building,
	        &matcher,
	        maxGap]( const IdentifiedData & data ) {
		       if ( matcher ) {
			       matcher->SetUp(std::get<1>(data),InteractionFrame::ConstPtr());
		       }

		       for ( const auto & pa : std::get<1>(data)->Positions ) {
			       auto & curTime = data.second->FrameTime;
			       if ( matcher && matcher->Match(pa.ID,0,{}) == false ) {
				       continue;
			       }

			       auto fi = building.find(pa.ID);
			       if ( fi != building.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last) == true
				            || curTime.Sub(fi->second.Last) > maxGap
				            || data.first != fi->second.SpaceID) {
					       auto res = fi->second.Terminate(pa.ID);
					       if ( res ) {
						       result.push_back(res);
					       }
					       building.erase(fi);
					       fi = building.end();
				       } else {
					       fi->second.Last = curTime;
					       fi->second.Durations.push_back(curTime.Sub(fi->second.Start).Nanoseconds());
					       fi->second.DataPoints.insert(fi->second.DataPoints.end(),
					                                    {pa.Position.x(),pa.Position.y(),pa.Angle});
				       }
			       }

			       if ( fi == building.end() ) {
				       BuildingTrajectory newStart;
				       newStart.SpaceID = data.first;
				       newStart.Start = curTime;
				       newStart.Last = curTime;
				       newStart.Durations = { 0 };
				       newStart.DataPoints = {pa.Position.x(),pa.Position.y(),pa.Angle};
				       building.insert(std::make_pair(pa.ID,newStart));;
			       }
		       }
	       };
}





std::function<void(const Query::InteractionData &)>
Query::BuildInteractions(std::vector<AntTrajectory::ConstPtr> & trajectories,
                         std::vector<AntInteraction::ConstPtr> & interactions,
                         BuildingTrajectoryData & currentTrajectories,
                         BuildingInteractionData & currentInteractions,
                         Duration maxGap,
                         const Matcher::Ptr & matcher) {
	return [&trajectories,
	        &interactions,
	        &currentTrajectories,
	        &currentInteractions,
	        &matcher,
	        maxGap]( const InteractionData & data ) {
		       if ( matcher ) {
			       matcher->SetUp(std::get<1>(data),std::get<2>(data));
		       }

		       std::vector<PositionedAnt> toTerminate;

		       auto & curTime = std::get<1>(data)->FrameTime;

		       for (  const auto & pa : std::get<1>(data)->Positions ) {
			       if ( matcher && matcher->Match(pa.ID,0,{}) == false ) {
				       continue;
			       }

			       auto fi = currentTrajectories.find(pa.ID);
			       if ( fi != currentTrajectories.end() ) {
				       if ( MonoIDMismatch(curTime,fi->second.Last)
				            || curTime.Sub(fi->second.Last) > maxGap
				            || std::get<0>(data) != fi->second.SpaceID) {
					       std::vector<InteractionID> toRemove;
					       for ( const auto & [IDs,interaction] : currentInteractions ) {
						       if ( IDs.first != pa.ID && IDs.second != pa.ID ) {
							       continue;
						       }
						       toRemove.push_back(IDs);
						       try {
							       interactions.push_back(interaction.Terminate(currentTrajectories.at(IDs.first),
							                                                    currentTrajectories.at(IDs.second)));
						       } catch ( const std::exception & ) {
						       }
					       }
					       toTerminate.push_back(pa);
					       for ( const auto & IDs : toRemove ) {
						       currentInteractions.erase(IDs);
					       }
				       } else {
					       fi->second.Last = curTime;
					       fi->second.Durations.push_back(curTime.Sub(fi->second.Start).Nanoseconds());
					       fi->second.DataPoints.insert(fi->second.DataPoints.end(),
					                                    {pa.Position.x(),pa.Position.y(),pa.Angle});
				       }
			       } else {
				       BuildingTrajectory newStart;
				       newStart.SpaceID = std::get<0>(data);
				       newStart.Start = curTime;
				       newStart.Last = curTime;
				       newStart.Durations = { 0 };
				       newStart.DataPoints = {pa.Position.x(),pa.Position.y(),pa.Angle};
				       currentTrajectories.insert(std::make_pair(pa.ID,newStart));;
			       }
		       }

		       for ( const auto & pa : toTerminate ) {
			       auto & curTraj = currentTrajectories.at(pa.ID);
			       trajectories.push_back(curTraj.Terminate(pa.ID));
			       curTraj.Start = curTime;
			       curTraj.Last = curTime;
			       curTraj.Durations.clear();
			       curTraj.Durations.push_back(0);
			       curTraj.DataPoints.clear();
			       curTraj.DataPoints.insert(curTraj.DataPoints.begin(),
			                                 {pa.Position.x(),pa.Position.y(),pa.Angle});
		       }


		       for ( const auto & pInter : std::get<2>(data)->Interactions ) {
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
					            interactions.push_back(fi->second.Terminate(currentTrajectories.at(pInter.IDs.first),
					                                                        currentTrajectories.at(pInter.IDs.second)));
					       } catch ( const std::exception & )  {
					       }
					       currentInteractions.erase(fi);
					       fi = currentInteractions.end();
				       } else {
					       fi->second.Last = curTime;
					       for ( const auto & type : pInter.InteractionTypes ) {
						       fi->second.Types.insert(type);
					       }
				       }
			       }

			       if ( fi == currentInteractions.end() ) {
				       BuildingInteraction newInter;
				       newInter.IDs = pInter.IDs;
				       newInter.Start = curTime;
				       newInter.Last = curTime;
				       for ( const auto & type : pInter.InteractionTypes ) {
					       newInter.Types.insert(type);
				       }
				       currentInteractions.insert(std::make_pair(newInter.IDs,newInter));
			       }

		       }

	       };

}


} // namespace priv
} // namespace myrmidon
} // namespace fort
