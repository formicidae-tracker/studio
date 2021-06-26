#include "QueryRunner.hpp"

#include <tbb/concurrent_queue.h>
#include <fort/myrmidon/utils/Defer.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


class DataLoader {
public:
	DataLoader(const Experiment::ConstPtr & experiment,
			   const QueryRunner::Args & args)
		: d_dataRanges(std::make_shared<DataRangeBySpaceID>())
		, d_rangeIterators(std::make_shared<RangesIteratorByID>())
		, d_dataIterators(std::make_shared<DataIteratorByID>()) {
		BuildRanges(experiment,args.Start,args.End,*d_dataRanges);
		for ( const auto & [spaceID,ranges] : *d_dataRanges ) {
			d_rangeIterators->insert(std::make_pair(spaceID,ranges.begin()));
			d_dataIterators->insert(std::make_pair(spaceID,std::move(ranges.begin()->first)));
		}
	}

	QueryRunner::RawData operator()(tbb::flow_control & fc) const {
		auto res = (*this)();
		if (std::get<0>(res) == 0 ) {
			fc.stop();
		}
		return res;
	}

	QueryRunner::RawData operator()() const {
		static int i = 0;
		SpaceID next(0);
		Time nextTime;
		for (  auto & [spaceID,dataIter] : *d_dataIterators ) {
			auto & rangeIterator = d_rangeIterators->at(spaceID);
			if ( rangeIterator == d_dataRanges->at(spaceID).cend() ) {
				continue;
			}
			if ( dataIter == d_rangeIterators->at(spaceID)->second ) {
				++rangeIterator;
				if ( rangeIterator == d_dataRanges->at(spaceID).cend() ) {
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
			return std::make_pair(0,RawFrame::ConstPtr());
		}

		auto & dataIter = d_dataIterators->at(next);

		auto res = *(dataIter);
		++dataIter;
		return std::make_pair(next,res);
	}

private:
	typedef std::pair<TrackingDataDirectory::const_iterator,
	                  TrackingDataDirectory::const_iterator> DataRange;
	typedef std::map<SpaceID,std::vector<DataRange>>         DataRangeBySpaceID;

	typedef std::map<SpaceID,std::vector<DataRange>::const_iterator> RangesIteratorByID;
	typedef std::map<SpaceID,TrackingDataDirectory::const_iterator>  DataIteratorByID;

	static void BuildRanges(const Experiment::ConstPtr & experiment,
							const Time & start,
							const Time & end,
							DataRangeBySpaceID & ranges) {
		const auto & spaces = experiment->Spaces();
		for ( const auto & [spaceID,space] : spaces ) {
			for ( const auto & tdd : space->TrackingDataDirectories() ) {
				TrackingDataDirectory::const_iterator ibegin(tdd->begin()),iend(tdd->end());
				if ( start.IsSinceEver() == false ) {
					if ( tdd->EndDate().Before(start) == true ) {
						continue;
					}
					if ( start.After(tdd->StartDate()) == true ) {
						ibegin = tdd->FrameAfter(start);
					}
				}
				if (end.IsForever() == false ) {
					if (end.Before(tdd->StartDate()) == true ) {
						continue;
					}
					iend = tdd->FrameAfter(end);
				}
				ranges[spaceID].push_back(std::make_pair(ibegin,iend));
			}
		}
	}


	std::shared_ptr<DataRangeBySpaceID>   d_dataRanges;
	std::shared_ptr<RangesIteratorByID>   d_rangeIterators;
	std::shared_ptr<DataIteratorByID>     d_dataIterators;

};


void QueryRunner::RunMultithread(const Experiment::ConstPtr & experiment,
								 const Args & args,
								 Finalizer finalizer) {

	DataLoader loader(experiment,args);

	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,loader);

	tbb::filter_t<RawData,Query::CollisionData>
		computeData(tbb::filter::parallel,
					QueryRunner::computeData(experiment,args));

	tbb::filter_t<Query::CollisionData,void>
		finalizeData(tbb::filter::serial_in_order,
					 finalizer);

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,
						   loadData & computeData & finalizeData);

}

void QueryRunner::RunSingleThread(const Experiment::ConstPtr & experiment,
								  const Args & args,
								  Finalizer finalizer) {

	DataLoader load(experiment,args);
	auto compute = QueryRunner::computeData(experiment,args);
	// we simply run in a single thread
	for (;;) {
		auto raw = load();
		if ( std::get<0>(raw) == 0 ) {
			break;
		}
		auto data  = compute(raw);
		finalizer(data);
	}
}

void QueryRunner::RunMultithreadFinalizeInCurrent(const Experiment::ConstPtr & experiment,
												  const Args & args,
												  Finalizer finalizer) {
	// we use a queue to retrieve all data in the main thread
	tbb::concurrent_bounded_queue<myrmidon::Query::CollisionData> queue;

	// we spawn a child process that will feed and close the queue
	auto process
		= [&queue,experiment,args]() {
			  RunMultithread(experiment,
							 args,
							 [&queue](const myrmidon::Query::CollisionData & data) {
								 queue.push(data);
							 });
			  queue.push(std::make_pair(nullptr,nullptr));
		  };

	std::thread go(process);

	// we consume the queue in the current thread
	for (;;) {
		Query::CollisionData v;
		queue.pop(v);
		if ( v.first == nullptr && v.second == nullptr ) {
			break;
		}
		finalizer(v);
	}

	// we wait for our thread to finish, should be the case as it is the one closing the queue
	go.join();
}


QueryRunner::Runner QueryRunner::RunnerFor(bool multithread,bool finalizerInCurrentThread) {
	if ( multithread == false ) {
		return RunSingleThread;
	}
	if ( finalizerInCurrentThread == false ) {
		return RunMultithread;
	}
	return RunMultithreadFinalizeInCurrent;
}


std::function<Query::CollisionData(const QueryRunner::RawData &)>
QueryRunner::computeData(const Experiment::ConstPtr & experiment,
						 const QueryRunner::Args & args) {
	auto identifier = experiment->Identifier()->Compile();
	if ( args.Collide == false && args.Localize == false ) {
		return [identifier](const RawData & raw) {
				   auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
				   return std::make_pair(identified,nullptr);
			   };
	}
	auto collider = experiment->CompileCollisionSolver();
	if ( args.Collide == false ) {
		return [identifier,collider] (const RawData & raw) {
				   auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
				   auto zoner = collider->ZonerFor(*identified);
				   for ( size_t i = 0; i < identified->Positions.rows(); ++i ) {
					   zoner->LocateAnt(identified->Positions.row(i));
				   }
				   return std::make_pair(identified,nullptr);
			   };
	}

	return [identifier,collider] (const RawData & raw) {
			   auto identified = std::get<1>(raw)->IdentifyFrom(*identifier,std::get<0>(raw));
			   auto collided = collider->ComputeCollisions(identified);
			   return std::make_pair(identified,collided);
		   };
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
