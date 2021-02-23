#pragma once

#include <fort/myrmidon/Time.hpp>

#include "CollisionSolver.hpp"
#include "Query.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class QueryRunner {
public:
	typedef std::pair<Space::ID,RawFrameConstPtr> RawData;

	struct Args {
		Time Start;
		Time End;
		bool Localize;
		bool Collide;
	};

	typedef std::function<void (const Query::CollisionData & data)> Finalizer;

	typedef std::function<void (const Experiment::ConstPtr &,const Args &,Finalizer)> Runner;

	static void RunMultithread(const Experiment::ConstPtr & experiment,
							   const Args & args,
							   Finalizer finalizer);

	static void RunMultithreadFinalizeInCurrent(const Experiment::ConstPtr & experiment,
												const Args & args,
												Finalizer finalizer);


	static void RunSingleThread(const Experiment::ConstPtr & experiment,
								const Args & args,
								Finalizer finalizer);



	static Runner RunnerFor(bool multithread,bool finalizerInCurrentThread);





private:


	static std::function<Query::CollisionData(const RawData &)> computeData(const Experiment::ConstPtr & experiment,
																			const Args & args);

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
