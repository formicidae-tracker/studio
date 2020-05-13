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
			if (!end) {
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
			            return std::make_pair(rawData.first,rawData.second->IdentifyFrom(*identifier));
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
			            auto identified = rawData.second->IdentifyFrom(*identifier);
			            auto interacted = solver->ComputeInteractions(rawData.first,identified);
			            return std::make_tuple(rawData.first,identified,interacted);
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

	tbb::filter_t<RawData,IdentifiedData>
		computeData(tbb::filter::parallel,
		            [identifier](const RawData & rawData ) {
			            return std::make_pair(rawData.first,rawData.second->IdentifyFrom(*identifier));
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
			       matcher->SetUp(data.second,InteractionFrame::ConstPtr());
		       }

		       for ( const auto & pa : data.second->Positions ) {
			       auto & curTime = data.second->FrameTime;
			       if ( matcher && matcher->Match(pa.ID,0,std::make_pair(0,0),curTime) == false ) {
				       continue;
			       }

			       auto fi = building.find(pa.ID);
			       if ( fi != building.end() ) {
				       if ( curTime.Sub(fi->second.Last) > maxGap || data.first != fi->second.SpaceID) {
					       auto res = fi->second.Terminate(pa.ID);
					       if ( res ) {
						       result.push_back(res);
					       }
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





} // namespace priv
} // namespace myrmidon
} // namespace fort
