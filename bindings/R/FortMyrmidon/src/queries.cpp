#include <FortMyrmidon/queries.h>

#include "time.h"
#include "experiment.h"
#include "matchers.h"
#include "duration.h"


#include <iostream>

#include <Rcpp.h>

using namespace fort::myrmidon;

std::tuple<Rcpp::IntegerVector,Rcpp::NumericVector,Rcpp::NumericVector,Rcpp::NumericVector>
fmPositionData_asR(const PositionedAntList & ants) {
		size_t n = ants.size();
		Rcpp::IntegerVector IDs(n);
		Rcpp::NumericVector X(n),Y(n),Angle(n);
		size_t i = 0;
		for ( const auto & ant : ants ) {
			IDs[i] = ant.ID;
			X[i] = ant.Position.x();
			Y[i] = ant.Position.y();
			Angle[i] = ant.Angle;
			++i;
		}
		return {IDs,X,Y,Angle};
}

SEXP fmIdentifiedFrames_asR(std::vector<IdentifiedFrame::ConstPtr> & frames, bool withZone ) {
	using namespace Rcpp;
	List positions(frames.size());
	DatetimeVector time(frames.size());
	IntegerVector width(frames.size()),height(frames.size()),space(frames.size());
	size_t i = 0;
	for ( auto & f : frames ) {
		auto [IDs,X,Y,Angle] = fmPositionData_asR(f->Positions);
		if ( withZone == true ) {
			positions[i] = DataFrame::create(_["antID"] = IDs,
			                                 _["x"] = X,
			                                 _["y"] = Y,
			                                 _["angle"] = Angle,
			                                 _["zone"] = IntegerVector(f->Zones.begin(),f->Zones.end()));
		} else {
			positions[i] = DataFrame::create(_["antID"] = IDs,
			                                 _["x"] = X,
			                                 _["y"] = Y,
			                                 _["angle"] = Angle);
		}

		time[i] = fmTime_asR(f->FrameTime);
		width[i] = f->Width;
		height[i] = f->Height;
		space[i] = f->Space;
		++i;
		f.reset();
	}

	return List::create(_["frames"] = DataFrame::create(_["time"] = time,
	                                                    _["space"] = space,
	                                                    _["width"] = width,
	                                                    _["height"] = height),
	                    _["positions"] = positions);
}



SEXP fmInteractionType_asR(const std::vector<InteractionType> & it ) {
	// ugly reinterpret cast, but the memory layout is right
	return Rcpp::IntegerMatrix(it.size(),
	                           2,
	                           reinterpret_cast<const std::vector<uint32_t>*>(&(it))->begin());
}

SEXP fmCollisionData_asR(std::vector<Query::CollisionData> & frames ) {
	using namespace Rcpp;
	List positions(frames.size());
	DatetimeVector time(frames.size());
	IntegerVector width(frames.size()),height(frames.size()),space(frames.size());

	std::vector<IntegerMatrix> types;
	std::vector<AntID> ant1IDs,ant2IDs;
	std::vector<size_t> frameRowIndex;
	std::vector<ZoneID> zone;
	size_t i = 0;
	for ( auto & [identified,collided] : frames) {
		auto [IDs,X,Y,Angle] = fmPositionData_asR(identified->Positions);
		positions[i] = Rcpp::DataFrame::create(Rcpp::_["antID"] = IDs,
		                                       Rcpp::_["x"] = X,
		                                       Rcpp::_["y"] = Y,
		                                       Rcpp::_["angle"] = Angle,
		                                       Rcpp::_["zone"] = IntegerVector(identified->Zones.cbegin(),identified->Zones.cend()));

		time[i] = fmTime_asR(identified->FrameTime);
		width[i] = identified->Width;
		height[i] = identified->Height;
		space[i] = identified->Space;

		for ( const auto & collision : collided->Collisions ) {
			frameRowIndex.push_back(i+1); // R index starts at 1, not 0.
			ant1IDs.push_back(collision.IDs.first);
			ant2IDs.push_back(collision.IDs.second);
			zone.push_back(collision.Zone);
			types.push_back(fmInteractionType_asR(collision.Types));
		}
		++i;
		identified.reset();
		collided.reset();
	}

	return List::create(_["frames"] = DataFrame::create(_["time"] = time,
	                                                    _["space"] = space,
	                                                    _["width"] = width,
	                                                    _["height"]  = height),
	                    _["positions"] = positions,
	                    _["collisions"] = DataFrame::create(_["ant1"] = IntegerVector(ant1IDs.begin(),ant1IDs.end()),
	                                                        _["ant2"] = IntegerVector(ant2IDs.begin(),ant2IDs.end()),
	                                                        _["zone"] = IntegerVector(zone.begin(),zone.end()),
	                                                        _["frameRowIndex"] = IntegerVector(frameRowIndex.begin(),
		                     frameRowIndex.end())),
	                    _["types"] = List(types.begin(),types.end()));
}

Rcpp::DataFrame fmAntTrajectoryPosition_asR(const AntTrajectory::ConstPtr & trajectory) {
	using namespace Rcpp;
	size_t nPoints = trajectory->Positions.rows();
#define numericVectorFromEigen(Var,matrix,col,size) NumericVector Var(&((matrix)(0,col)),&((matrix)(0,col))+size)
 	numericVectorFromEigen(time,trajectory->Positions,0,nPoints);
	numericVectorFromEigen(x,trajectory->Positions,1,nPoints);
 	numericVectorFromEigen(y,trajectory->Positions,2,nPoints);
 	numericVectorFromEigen(angle,trajectory->Positions,3,nPoints);
	if ( trajectory->Zones.empty() == false ) {
		return DataFrame::create(_["time"] = time,
		                         _["x"] = x,
		                         _["y"] = y,
		                         _["angle"] = angle,
		                         _["zone"] = IntegerVector(trajectory->Zones.cbegin(),
		                                                   trajectory->Zones.cend()));
	}
	return DataFrame::create(_["time"] = time,
	                         _["x"] = x,
	                         _["y"] = y,
	                         _["angle"] = angle);
}


std::tuple<Rcpp::DataFrame,Rcpp::List>
fmAntTrajectories_asR(const std::vector<AntTrajectory::ConstPtr> & trajectories) {
	using namespace Rcpp;
	auto n = trajectories.size();
	IntegerVector antID(n),space(n);
	DatetimeVector start(n);
	List positions(n);
	size_t i = 0;
	for ( const auto & trajectory : trajectories ) {
		antID[i] = trajectory->Ant;
		space[i] = trajectory->Space;
		start[i] = fmTime_asR(trajectory->Start);
		positions[i] = fmAntTrajectoryPosition_asR(trajectory);
		++i;
	}

	return {DataFrame::create(_["antID"] = antID,
	                          _["start"] = start,
	                          _["space"] = space),
	        positions};

}

std::tuple<Rcpp::DataFrame,Rcpp::List,Rcpp::List,Rcpp::List>
fmAntInteractions_asR(std::vector<AntInteraction::ConstPtr> & interactions) {
	using namespace Rcpp;
	auto n = interactions.size();
	List ant1Trajectory(n),ant2Trajectory(n),types(n);
	IntegerVector ant1(n),ant2(n),space(n);
	DatetimeVector start(n),end(n);
	size_t i = 0;
	for ( auto & interaction : interactions ) {
		ant1[i] = interaction->IDs.first;
		ant2[i] = interaction->IDs.second;
		start[i] = fmTime_asR(interaction->Start);
		end[i] = fmTime_asR(interaction->End);
		space[i] = interaction->Space;

		types[i] = fmInteractionType_asR(interaction->Types);

		ant1Trajectory[n] = fmAntTrajectoryPosition_asR(interaction->Trajectories.first);
		ant2Trajectory[n] = fmAntTrajectoryPosition_asR(interaction->Trajectories.second);

		++i;
		interaction.reset();
	}

	return {DataFrame::create(_["ant1"] = ant1,
	                          _["ant2"] = ant2,
	                          _["start"] = start,
	                          _["end"] = end,
	                          _["space"] = space),
	        types,
	        ant1Trajectory,
	        ant2Trajectory};
}

std::tuple<Rcpp::DataFrame,Rcpp::List,Rcpp::List,Rcpp::List>
fmAntInteractionsSummarized_asR(std::vector<AntInteraction::ConstPtr> & interactions,
                               std::vector<double> & meanAnt1,
                               std::vector<double> & meanAnt2,
                               const std::vector<std::vector<ZoneID>> & zoneAnt1,
                               const std::vector<std::vector<ZoneID>> & zoneAnt2) {
	using namespace Rcpp;
	auto n = interactions.size();
	List types(n),zones1(n),zones2(n);
	size_t i = 0;
	IntegerVector ant1(n),ant2(n),space(n);
	DatetimeVector start(n),end(n);


	for ( auto & interaction : interactions) {
		ant1[i] = interaction->IDs.first;
		ant2[i] = interaction->IDs.second;
		start[i] = fmTime_asR(interaction->Start);
		end[i] = fmTime_asR(interaction->End);
		space[i] = interaction->Space;

		types[i] = fmInteractionType_asR(interaction->Types);

		zones1[i] = NumericVector(zoneAnt1[i].cbegin(),
		                          zoneAnt1[i].cend());
		zones2[i] = NumericVector(zoneAnt2[i].cbegin(),
		                          zoneAnt2[i].cend());
		++i;
		interaction.reset();
	}

	//first go from row-major to column-major using eigen.
	Eigen::MatrixXd mean1 = Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&meanAnt1[0],n,3);
	meanAnt1.clear();
	Eigen::MatrixXd mean2 = Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&meanAnt2[0],n,3);
	meanAnt2.clear();

	// now duplicates again the data to R, from column major order
	numericVectorFromEigen(x1mean,mean1,0,n);
	numericVectorFromEigen(y1mean,mean1,1,n);
	numericVectorFromEigen(angle1mean,mean1,2,n);

	numericVectorFromEigen(x2mean,mean2,0,n);
	numericVectorFromEigen(y2mean,mean2,1,n);
	numericVectorFromEigen(angle2mean,mean2,2,n);

	// now we can put everything in data.frame
	return { DataFrame::create(_["ant1"] = ant1,
	                           _["ant2"] = ant2,
	                           _["start"] = start,
	                           _["end"] = end,
	                           _["space"] = space,
	                           _["x_1_mean"] = x1mean,
	                           _["y_1_mean"] = y1mean,
	                           _["angle_1_mean"] = angle1mean,
	                           _["x_2_mean"] = x2mean,
	                           _["y_2_mean"] = y2mean,
	                           _["angle_2_mean"] = angle2mean),
	         types,
	         zones1,
	         zones2 };
}

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
		TagName[nTags] = fort::myrmidon::FormatTagID(stat.ID);
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

	auto res = Rcpp::DataFrame::create(Rcpp::_["TagID"] = TagID,
	                                   Rcpp::_["First Seen"] = FirstSeen,
	                                   Rcpp::_["Last Seen"] = LastSeen,
	                                   Rcpp::_["Count"] = Count,
	                                   Rcpp::_["Multiple Seen"] = Multiple,
	                                   Rcpp::_["Gap < 500ms"] = Gap500,
	                                   Rcpp::_["Gap < 1s"] = Gap1s,
	                                   Rcpp::_["Gap < 10s"] = Gap10s,
	                                   Rcpp::_["Gap < 1m"] = Gap1m,
	                                   Rcpp::_["Gap < 10m"] = Gap10m,
	                                   Rcpp::_["Gap < 1h"] = Gap1h,
	                                   Rcpp::_["Gap < 10h"] = Gap10h,
	                                   Rcpp::_["Gap >= 10h"] = GapMore);
	res.names() = TagName;
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
	SET_PD("R conversion");
	auto res = fmIdentifiedFrames_asR(res_,computeZones);
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
	return Rcpp::List::create(Rcpp::_["trajectorySummary"] = summary,
	                          Rcpp::_["trajectories"] = trajectories);
}

double meanUS(const std::vector<Duration> & durations) {
	double res = 0.0 ;
	for ( const auto & d: durations ) { res += d.Microseconds()/durations.size(); }
	return res;
}


std::pair<Eigen::Vector3d,std::vector<ZoneID>> SummarizeTrajectory(const AntTrajectory::ConstPtr & trajectory) {
	std::set<ZoneID> set(trajectory->Zones.begin(),trajectory->Zones.end());
	return {trajectory->Positions.block(0,1,trajectory->Positions.rows(),3).colwise().mean(),
	        {set.begin(),set.end()}};
}

SEXP fmQueryComputeAntInteractions(const CExperiment & experiment,
                                   const Time::ConstPtr & startTime,
                                   const Time::ConstPtr & endTime,
                                   const Duration & maximuGap,
                                   const Matcher::Ptr & matcher,
                                   bool reportGlobalTrajectories,
                                   bool reportLocalTrajectories,
                                   bool singleThreaded,
                                   bool showProgress ) {

	std::vector<AntTrajectory::ConstPtr> resTrajectories;
	std::function<void (const AntTrajectory::ConstPtr &)> storeTrajectories =
		[&resTrajectories](const AntTrajectory::ConstPtr & trajectory ) {
			resTrajectories.push_back(trajectory);
		};
	if ( reportGlobalTrajectories ==  false ) {
		//drop the data instead of storing it
		storeTrajectories = [](const AntTrajectory::ConstPtr &) {
		                  };
	}

	std::vector<AntInteraction::ConstPtr>  resInteractions;
	std::vector<double> meanAnt1,meanAnt2;
	std::vector<std::vector<ZoneID>> zone1,zone2;

	std::function<void (const AntInteraction::ConstPtr &)> storeInteractions =
		[&resInteractions](const AntInteraction::ConstPtr & interaction) {
			resInteractions.push_back(interaction);
		};

	if ( reportLocalTrajectories == false ) {
		storeInteractions =
			[&](const AntInteraction::ConstPtr & interaction) {
				auto smallerInteraction = std::make_shared<AntInteraction>(AntInteraction{
					    .IDs = interaction->IDs,
					    .Types = interaction->Types,
					    .Start = interaction->Start,
					    .End = interaction->End,
					    .Space = interaction->Space,
					});

				resInteractions.push_back(smallerInteraction);

				auto [m1,z1] = SummarizeTrajectory(interaction->Trajectories.first);
				auto [m2,z2] = SummarizeTrajectory(interaction->Trajectories.second);

				meanAnt1.push_back(m1.x());
				meanAnt1.push_back(m1.y());
				meanAnt1.push_back(m1.z());
				meanAnt2.push_back(m2.x());
				meanAnt2.push_back(m2.y());
				meanAnt2.push_back(m2.z());

				zone1.push_back(z1);
				zone2.push_back(z2);

			};
	}

	DECLARE_PD();
	SET_PD("Processing");
	Query::ComputeAntInteractionsFunctor(experiment,
	                                     storeTrajectories,
	                                     storeInteractions,
	                                     startTime,
	                                     endTime,
	                                     maximuGap,
	                                     matcher,
	                                     singleThreaded);
	Rcpp::List res;
	if (reportGlobalTrajectories == true ) {
		SET_PD("R trajectory conversion");
		auto [summary,trajectories] = fmAntTrajectories_asR(resTrajectories);
		res["summaryTrajectory"] = summary;
		res["trajectories"] = trajectories;
	}

	SET_PD("R interaction conversion");
	if (reportLocalTrajectories == true ) {
		auto [interactions,types,t1,t2] = fmAntInteractions_asR(resInteractions);
		res["interactions"] = interactions;
		res["types"] = types;
		res["ant1Trajectory"] = t1;
		res["ant2Trajectory"] = t2;
	} else {
		auto [interactions,types,z1,z2] = fmAntInteractionsSummarized_asR(resInteractions,
		                                                                  meanAnt1,
		                                                                  meanAnt2,
		                                                                  zone1,
		                                                                  zone2);
		res["interactions"] = interactions;
		res["types"] = types;
		res["ant1zones"] = z1;
		res["ant2zones"] = z2;
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
