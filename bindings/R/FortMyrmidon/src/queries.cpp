#include <FortMyrmidon/queries.h>

#include "types.h"

#include "time.h"
#include "experiment.h"
#include "matchers.h"
#include "duration.h"


#include <iostream>

#include <Rcpp.h>

using namespace fort::myrmidon;




Rcpp::DataFrame fmQuery_computeMeasurementFor(const CExperiment & experiment,
                                              AntID antID,
                                              MeasurementTypeID mTypeID) {
	auto cMeasurements = Query::ComputeMeasurementFor(experiment,antID, mTypeID);

	Rcpp::DatetimeVector times(cMeasurements.size());
	Rcpp::NumericVector lengths(cMeasurements.size());
	for( size_t i = 0; i < cMeasurements.size(); ++i ) {
		times[i] = fmTime_asR(cMeasurements[i].MTime);
		lengths[i] = cMeasurements[i].LengthMM;
	}

	return Rcpp::DataFrame::create(Rcpp::_["date"] = times,
	                               Rcpp::_["length (mm)"] = lengths);
}


SEXP fmTagStatistics_asR(const TagStatistics::ByTagID & tagStats ) {
	size_t nTags  = tagStats.size();
	Rcpp::IntegerVector TagID(nTags),Count(nTags),Multiple(nTags),Gap500(nTags),Gap1s(nTags),Gap10s(nTags),Gap1m(nTags),Gap10m(nTags),Gap1h(nTags),Gap10h(nTags),GapMore(nTags);
	Rcpp::CharacterVector TagName(nTags);
	Rcpp::DatetimeVector FirstSeen(nTags),LastSeen(nTags);
	size_t i = 0;
	for(const auto & [tagID,stat] : tagStats) {
		TagID[i] = stat.ID;
		TagName[i] = fort::myrmidon::FormatTagID(stat.ID);
		FirstSeen[i] = fmTime_asR(stat.FirstSeen);
		LastSeen[i] = fmTime_asR(stat.LastSeen);
		Count[i] = stat.Counts(TagStatistics::TOTAL_SEEN);
		Multiple[i] = stat.Counts(TagStatistics::MULTIPLE_SEEN);
		Gap500[i] = stat.Counts(TagStatistics::GAP_500MS);
		Gap1s[i] = stat.Counts(TagStatistics::GAP_1S);
		Gap10s[i] = stat.Counts(TagStatistics::GAP_10S);
		Gap1m[i] = stat.Counts(TagStatistics::GAP_1M);
		Gap10m[i] = stat.Counts(TagStatistics::GAP_10M);
		Gap1h[i] = stat.Counts(TagStatistics::GAP_1H);
		Gap10h[i] = stat.Counts(TagStatistics::GAP_10H);
		GapMore[i] = stat.Counts(TagStatistics::GAP_MORE);
		++i;
	}

	auto res = Rcpp::DataFrame::create(Rcpp::_["tagDecimalValue"] = TagID,
	                                   Rcpp::_["firstSeen"] = FirstSeen,
	                                   Rcpp::_["lastSeen"] = LastSeen,
	                                   Rcpp::_["count"] = Count,
	                                   Rcpp::_["multipleSeen"] = Multiple,
	                                   Rcpp::_["gap500ms"] = Gap500,
	                                   Rcpp::_["gap1s"] = Gap1s,
	                                   Rcpp::_["gap10s"] = Gap10s,
	                                   Rcpp::_["gap1m"] = Gap1m,
	                                   Rcpp::_["gap10m"] = Gap10m,
	                                   Rcpp::_["gap1h"] = Gap1h,
	                                   Rcpp::_["gap10h"] = Gap10h,
	                                   Rcpp::_["gap10h"] = GapMore);

	res.attr("row.names") = TagName;
	return res;
}


SEXP fmQuery_computeTagStatistics(const CExperiment & experiment) {
	return fmTagStatistics_asR(Query::ComputeTagStatistics(experiment));
}



class ProgressDisplayerIF {
public :
	virtual ~ProgressDisplayerIF(){}
	virtual void ShowProgress(const Time & t) {}

};

class ProgressDisplayer : public ProgressDisplayerIF{
private :
	Duration d_duration;
	Time d_last,d_computeStart,d_lastShown,d_start,d_end;
	std::string d_what;
public :
	ProgressDisplayer(const CExperiment & experiment,
	                  const Time::ConstPtr & start,
	                  const Time::ConstPtr & end,
	                  const std::string & what,
	                  Duration d = Duration::Hour)
		: d_duration(d)
		, d_what(what) {
		if ( !start || !end ) {
			auto info = experiment.GetDataInformations();
			if ( !start  ) {
				d_start = info.Start;
			} else {
				d_start = *start;
			}
			if ( !end ) {
				d_end = info.End;
			} else {
				d_end = *end;
			}
		} else {
			d_start = *start;
			d_end = *end;
		}
		d_last = d_start;
		d_lastShown = Time::Now();
		d_computeStart = d_lastShown;
	}

	virtual ~ProgressDisplayer() {
		std::cerr << d_what << " took " << Time::Now().Sub(d_computeStart) << "\n";
	}

	void ShowProgress(const Time & t) override{
		auto ellapsed = t.Sub(d_last);
		if ( ellapsed < d_duration ) {
			return;
		}
		d_last = t;
		auto now = Time::Now();
		auto computationTime = now.Sub(d_lastShown);
		auto reminder = d_end.Sub(t);
		d_lastShown = now;
		std::cerr << "Processed frame at " << t
		          << ", computed "
		          << ellapsed
		          << " in "
		          << computationTime
		          << ". Remind "
		          << reminder
		          << ", ETA " << Duration((reminder.Seconds() / ellapsed.Seconds()) * computationTime.Seconds() * 1e9)
		          << "\n";
	}

};

#define DECLARE_PD() \
	auto pd = new ProgressDisplayerIF()
#define CLEAR_PD() \
	delete pd

#define SET_PD(What) \
	if ( showProgress == true ) { \
		CLEAR_PD(); \
		pd = new ProgressDisplayer(experiment,startTime,endTime,What); \
	} \


SEXP fmQueryIdentifyFrames(const fort::myrmidon::CExperiment & experiment,
                           const fort::myrmidon::Time::ConstPtr & startTime,
                           const fort::myrmidon::Time::ConstPtr & endTime,
                           bool computeZones = false,
                           bool singleThread = false,
                           bool showProgress = false) {
	std::vector<IdentifiedFrame::ConstPtr> res_;
	DECLARE_PD();
	SET_PD("Processing");
	Query::IdentifyFramesFunctor(experiment,
	                             [&res_,pd](const IdentifiedFrame::ConstPtr & frame) {
		                             res_.push_back(frame);
		                             pd->ShowProgress(frame->FrameTime);
	                             },
	                             startTime,
	                             endTime,
	                             computeZones,
	                             true);
	SET_PD("R conversion");\
	SEXP res;
	if ( computeZones == true) {
		res = fmIdentifiedFramesWithZones_asR(res_);
	} else {
		res = fmIdentifiedFrames_asR(res_);
	}
	CLEAR_PD();
	return res;
}

SEXP fmQueryCollideFrames(const fort::myrmidon::CExperiment & experiment,
                          const fort::myrmidon::Time::ConstPtr & startTime,
                          const fort::myrmidon::Time::ConstPtr & endTime,
                          bool singleThread,
                          bool showProgress) {
	std::vector<Query::CollisionData> res_;
	DECLARE_PD();
	SET_PD("Processing");
	Query::CollideFramesFunctor(experiment,
	                            [&res_,pd](const Query::CollisionData & data) {
		                            res_.push_back(data);
		                            pd->ShowProgress(std::get<0>(data)->FrameTime);
	                            },
	                            startTime,
	                            endTime,
	                            true);
	SET_PD("R conversion");
	auto res = fmCollisionData_asR(res_);
	CLEAR_PD();
	return res;
}


SEXP fmQueryComputeAntTrajectories(const CExperiment & experiment,
                                   const Time::ConstPtr & startTime,
                                   const Time::ConstPtr & endTime,
                                   const Duration & maximuGap,
                                   const Matcher::Ptr & matcher,
                                   bool computeZones,
                                   bool singleThreaded,
                                   bool showProgress) {
	std::vector<AntTrajectory::ConstPtr> res_;
	DECLARE_PD();
	SET_PD("Processing");
	Query::ComputeAntTrajectoriesFunctor(experiment,
	                                     [&res_,pd](const AntTrajectory::ConstPtr & at) {
		                                     res_.push_back(at);
		                                     pd->ShowProgress(at->End());
	                                     },
	                                     startTime,
	                                     endTime,
	                                     maximuGap,
	                                     matcher,
	                                     computeZones,
	                                     singleThreaded);
	SET_PD("R Conversion");
	auto [summary,trajectories] = fmAntTrajectories_asR(res_);
	CLEAR_PD();
	return Rcpp::List::create(Rcpp::_["trajectory_summary"] = summary,
	                          Rcpp::_["trajectories"] = trajectories);
}

double meanUS(const std::vector<Duration> & durations) {
	double res = 0.0 ;
	for ( const auto & d: durations ) { res += d.Microseconds()/durations.size(); }
	return res;
}


std::pair<Eigen::Vector3d,std::string> SummarizeTrajectory(const AntTrajectorySegment & segment) {
	std::set<ZoneID> set(segment.Trajectory->Zones.begin(),segment.Trajectory->Zones.end());
	std::ostringstream oss;
	for ( const auto & zone : set ) {
		oss << zone << ",";
	}
	std::string zones(oss.str());
	zones.pop_back();

	return {segment.Trajectory->Positions.block(segment.Begin,1,segment.End-segment.Begin,3).colwise().mean(),
	        zones};
}

SEXP fmQueryComputeAntInteractions(const CExperiment & experiment,
                                   const Time::ConstPtr & startTime,
                                   const Time::ConstPtr & endTime,
                                   const Duration & maximuGap,
                                   const Matcher::Ptr & matcher,
                                   bool reportTrajectories,
                                   bool singleThreaded,
                                   bool showProgress ) {

	std::vector<AntTrajectory::ConstPtr> resTrajectories;
	std::vector<AntInteraction::ConstPtr>  resInteractions;

	std::map<const AntTrajectory*,std::pair<std::vector<size_t>,std::vector<size_t>>> needsIndexing;
	TrajectorySummary ant1Summary,ant2Summary;
	TrajectoryIndexing ant1Indexing,ant2Indexing;

	DECLARE_PD();

	SET_PD("Processing");

	std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectories =
		[&resTrajectories,&pd,&needsIndexing,&ant1Indexing,&ant2Indexing](const AntTrajectory::ConstPtr & trajectory ) {
			// R indexes starts at 1
			size_t trajectoryIndex = resTrajectories.size() +1 ;
			resTrajectories.push_back(trajectory);
			for ( const auto & interactionIndex : needsIndexing[trajectory.get()].first ) {
				ant1Indexing.RowIndexes[interactionIndex] = trajectoryIndex;
			}
			for ( const auto & interactionIndex : needsIndexing[trajectory.get()].second ) {
				ant2Indexing.RowIndexes[interactionIndex] = trajectoryIndex;
			}
			needsIndexing.erase(trajectory.get());
			pd->ShowProgress(trajectory->End());
		};
	if ( reportTrajectories ==  false ) {
		//drop the data instead of storing it
		storeTrajectories = [&pd](const AntTrajectory::ConstPtr & trajectory) {
			                    pd->ShowProgress(trajectory->End());
		                  };
	}



	std::function<void (const AntInteraction::ConstPtr &)> storeInteractions =
		[&resInteractions,&needsIndexing,&ant1Indexing,&ant2Indexing](const AntInteraction::ConstPtr & interaction) {
			size_t interactionIndex = resInteractions.size();
			resInteractions.push_back(interaction);
			needsIndexing[interaction->Trajectories.first.Trajectory.get()].first.push_back(interactionIndex);
			needsIndexing[interaction->Trajectories.second.Trajectory.get()].second.push_back(interactionIndex);
			ant1Indexing.Push(interaction->Trajectories.first);
			ant2Indexing.Push(interaction->Trajectories.second);
			static Time lastTime;
			if ( interaction->End.Sub(lastTime) > 10 * Duration::Minute ) {
				lastTime = interaction->End;
				std::cerr << "indexStageAreaSize: " << needsIndexing.size() << std::endl;

			}

		};

	if ( reportTrajectories == false ) {
		storeInteractions =
			[&](const AntInteraction::ConstPtr & interaction) {
				auto smallerInteraction = std::make_shared<AntInteraction>();
				smallerInteraction->IDs = interaction->IDs;
				smallerInteraction->Types = interaction->Types;
				smallerInteraction->Start = interaction->Start;
				smallerInteraction->End = interaction->End;
				smallerInteraction->Space = interaction->Space;

				resInteractions.push_back(smallerInteraction);

				auto [m1,z1] = SummarizeTrajectory(interaction->Trajectories.first);
				auto [m2,z2] = SummarizeTrajectory(interaction->Trajectories.second);

				ant1Summary.Push(m1,z1);
				ant2Summary.Push(m2,z2);

			};
	}

	Query::ComputeAntInteractionsFunctor(experiment,
	                                     storeTrajectories,
	                                     storeInteractions,
	                                     startTime,
	                                     endTime,
	                                     maximuGap,
	                                     matcher,
	                                     singleThreaded);

	if ( needsIndexing.size() != 0 ) {
		throw std::runtime_error("Missing " + std::to_string(needsIndexing.size()) + " index(es)");
	}

	Rcpp::List res;
	if (reportTrajectories == true ) {
		SET_PD("R trajectory conversion");
		auto [summary,trajectories] = fmAntTrajectories_asR(resTrajectories);
		res["summaryTrajectory"] = summary;
		res["trajectories"] = trajectories;
	}

	SET_PD("R interaction conversion");
	if (reportTrajectories == true ) {
		res["interactions"] = fmAntInteractions_asR(resInteractions,
		                                            ant1Indexing,
		                                            ant2Indexing);
	} else {
		res["interactions"] = fmAntInteractionsSummarized_asR(resInteractions,
		                                                      ant1Summary,
		                                                      ant2Summary);
	}
	CLEAR_PD();
	return res;
}


RCPP_MODULE(queries) {

	Rcpp::function("fmQueryComputeMeasurementFor",&fmQuery_computeMeasurementFor);
	Rcpp::function("fmQueryComputeTagStatistics",&fmQuery_computeTagStatistics);
	Rcpp::function("fmQueryIdentifyFramesC",&fmQueryIdentifyFrames);
	Rcpp::function("fmQueryCollideFramesC",&fmQueryCollideFrames);
	Rcpp::function("fmQueryComputeAntTrajectoriesC",&fmQueryComputeAntTrajectories);
	Rcpp::function("fmQueryComputeAntInteractionsC",&fmQueryComputeAntInteractions);

}
