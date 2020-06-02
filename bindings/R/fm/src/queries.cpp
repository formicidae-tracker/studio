#include "queries.h"

#include "time.h"
#include "experiment.h"
#include "matchers.h"
#include "duration.h"


#include <iostream>

#include <Rcpp.h>

using namespace fort::myrmidon;

SEXP fmIdentifiedFrame_asR(const IdentifiedFrame::ConstPtr & frame) {
	size_t n = frame->Positions.size();
	Rcpp::IntegerVector IDs(n);
	Rcpp::NumericVector X(n),Y(n),Angle(n);
	size_t i = 0;
	for ( const auto & ant : frame->Positions ) {
		IDs[i] = ant.ID;
		X[i] = ant.Position.x();
		Y[i] = ant.Position.y();
		Angle[i] = ant.Angle;
		++i;
	}


	Rcpp::List Data;
	Rcpp::CharacterVector colNames({"AntID","X","Y","Angle"}); \
	Data = Rcpp::List({IDs,X,Y,Angle});
	if ( frame->Zones.empty() == false ) {
		Data.push_back(Rcpp::IntegerVector(frame->Zones.begin(),frame->Zones.end()));
		colNames.push_back("ZoneID");
	}


	Data.attr("names") = colNames;
	Data.attr("row.names") = Rcpp::IntegerVector::create(NA_INTEGER,n);
	Data.attr("class") = "data.frame";
	Rcpp::S4 res("fmIdentfiedFrame");

	res.slot("frameTime") = fmTime_asR(frame->FrameTime);
	res.slot("width") = frame->Width;
	res.slot("height") = frame->Height;
	res.slot("space") = frame->Space;
	res.slot("data") = Data;
	return res;
}



IdentifiedFrame::ConstPtr IdentifiedFrame_debug() {
	auto f = std::make_shared<IdentifiedFrame>();
	f->FrameTime = Time::Now();
	f->Space = 42;
	f->Width = 1920;
	f->Height = 1080;
	f->Positions.push_back(PositionedAnt{Eigen::Vector2d(0,0),0,1});
	f->Positions.push_back(PositionedAnt{Eigen::Vector2d(1,1),-0.1,2});
	f->Zones.push_back(0);
	f->Zones.push_back(2);

	return f;
}

SEXP fmIdentifiedFrame_debug() {
	return fmIdentifiedFrame_asR(IdentifiedFrame_debug());
}

struct fmCollision {
	AntID  Ant1;
	AntID  Ant2;
	ZoneID Zone;
	Rcpp::List             InteractionTypes;
	void Show() const {
		Rcpp::Rcout << "fmCollision (\n"
		            << "  ant1 = " << Ant1 << "\n"
		            << "  ant2 = " << Ant2 << "\n"
		            << "  zone = " << Zone << "\n"
		            << "  interactionTypes = ";
		Rcpp::Function("str")(InteractionTypes);
		Rcpp::Rcout << ")\n";
	}

	fmCollision(const Collision & c)
		: Ant1(c.IDs.first)
		, Ant2(c.IDs.second)
		, Zone(c.Zone)
		, InteractionTypes(c.InteractionTypes.size()) {
		for ( size_t i = 0 ; i < c.InteractionTypes.size(); ++i ) {
			const auto & t = c.InteractionTypes[i];
			InteractionTypes[i] = Rcpp::IntegerVector({(int)t.first,(int)t.second});
		}
	}
};

SEXP fmInteractionTypes_asR(const std::vector<InteractionType> & it ) {
	// ugly reinterpret cast, but the memory layout is right
	return Rcpp::IntegerMatrix(it.size(),
	                           2,
	                           reinterpret_cast<const std::vector<uint32_t>*>(&(it))->begin());
}


SEXP fmCollision_asR(const Collision & c) {
	Rcpp::S4 res("fmCollision");
	res.slot("ant1") = c.IDs.first;
	res.slot("ant2") = c.IDs.second;
	res.slot("zone") = c.Zone;
	res.slot("interactionTypes") = fmInteractionTypes_asR(c.InteractionTypes);
	return res;
}

Collision Collision_debug() {
	return Collision{
		.IDs = std::make_pair(3,4),.InteractionTypes = {{1,1},{2,1},{1,3}},.Zone= 51};
}

SEXP fmCollision_debug() {
	return fmCollision_asR(Collision_debug());
}



SEXP fmCollisionFrame_asR(const CollisionFrame::ConstPtr & frame ) {
	Rcpp::S4 res("fmCollisionFrame");
	res.slot("frameTime") = fmTime_asR(frame->FrameTime);
	res.slot("space") = frame->Space;
	res.slot("collisions") = Rcpp::List::import_transform(frame->Collisions.cbegin(),
	                                                      frame->Collisions.cend(),
	                                                      &fmCollision_asR);
	return res;
}


CollisionFrame::ConstPtr CollisionFrame_debug() {
	auto f = std::make_shared<CollisionFrame>();
	f->FrameTime = Time::Now();
	f->Space = 42;
	for ( size_t i = 0 ; i < 3; ++i) {
		f->Collisions.push_back(Collision_debug());
	}
	return f;
}

SEXP fmCollisionFrame_debug() {
	return fmCollisionFrame_asR(CollisionFrame_debug());
}


SEXP fmAntTrajectory_asR(const AntTrajectory::ConstPtr & at) {
	size_t nPoints = at->Data.rows();

#define numericVectorFromEigen(Var,matrix,col,size) Rcpp::NumericVector Var(&((matrix)(0,col)),&((matrix)(0,col))+size)
	numericVectorFromEigen(Times,at->Data,0,nPoints);
	numericVectorFromEigen(Xs,at->Data,1,nPoints);
	numericVectorFromEigen(Ys,at->Data,2,nPoints);
	numericVectorFromEigen(Angles,at->Data,3,nPoints);
#undef numericVectorFromEigen
	Rcpp::List data({Times,Xs,Ys,Angles});
	Rcpp::CharacterVector names = {"Time (s)","X","Y","Angle"};
	if ( at->Zones.empty() == false ) {
		data.push_back(Rcpp::IntegerVector(at->Zones.cbegin(),at->Zones.cend()));
		names.push_back("Zone");
	}
	data.attr("names") = names;
	data.attr("row.names") = Rcpp::IntegerVector::create(NA_INTEGER,nPoints);
	data.attr("class") = "data.frame";


	Rcpp::S4 res("fmAntTrajectory");
	res.slot("ant") = at->Ant;
	res.slot("start") = fmTime_asR(at->Start);
	res.slot("space") = at->Space;
	res.slot("data") = data;
	return res;
}

AntTrajectory::ConstPtr AntTrajectory_debug() {
	auto res = std::make_shared<AntTrajectory>();
	res->Ant = 3;
	res->Space = 42;
	res->Start = Time::Now();
	res->Data.resize(10,4);
	for ( size_t i = 0; i < 10 ; ++i ) {
		res->Data(i,0) = i*0.1;
		res->Data(i,1) = 1.0 * i;
		res->Data(i,2) = -1.0 * i;
		res->Data(i,3) = i * 0.1;
		res->Zones.push_back(i > 4 ? 51 : 0);
	}
	return res;
}

SEXP fmAntTrajectory_debug() {
	return fmAntTrajectory_asR(AntTrajectory_debug());
}


SEXP fmAntInteraction_asR(const AntInteraction::ConstPtr & ai) {
	Rcpp::S4 res("fmAntInteraction");
	res.slot("ant1") = ai->IDs.first;
	res.slot("ant2") = ai->IDs.second;
	res.slot("ant1Trajectory") = fmAntTrajectory_asR(ai->Trajectories.first);
	res.slot("ant2Trajectory") = fmAntTrajectory_asR(ai->Trajectories.second);
	res.slot("start") = fmTime_asR(ai->Start);
	res.slot("end") = fmTime_asR(ai->End);
	res.slot("types") = fmInteractionTypes_asR(ai->Types);
	return res;
}

AntInteraction::ConstPtr AntInteraction_debug() {
	auto res = std::make_shared<AntInteraction>();
	res->IDs = {3,4};
	res->Types = { {1,2},{1,3},{2,1} };
	res->Trajectories = {AntTrajectory_debug(),AntTrajectory_debug()};
	res->Start = Time::Now();
	res->End = res->Start.Add(10 * Duration::Second);
	return res;
}

SEXP fmAntInteraction_debug() {
	return fmAntInteraction_asR(AntInteraction_debug());
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
	Rcpp::DatetimeVector FirstSeen(nTags),LastSeen(nTags);
	size_t i = 0;
	for(const auto & [tagID,stat] : tagStats) {
		TagID[i] = stat.ID;
		FirstSeen[i] = stat.FirstSeen.ToTimeT();
		LastSeen[i] = stat.LastSeen.ToTimeT();
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

	return Rcpp::DataFrame::create(Rcpp::_["TagID"] = TagID,
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

}


SEXP fmQuery_computeTagStatistics(const CExperiment & experiment) {
	return fmTagStatistics_asR(Query::ComputeTagStatistics(experiment));
}


class ProgressDisplayer {
private :
	Duration d_duration;
	Time d_last,d_lastShown,d_start,d_end;
public :
	ProgressDisplayer(const CExperiment & experiment,
	                  const Time::ConstPtr & start,
	                  const Time::ConstPtr & end,
	                  Duration d = Duration::Hour )
		: d_duration(d) {
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
	}

	void ShowProgress(const Time & t) {
		auto ellapsed = t.Sub(d_last);
		if ( ellapsed < d_duration ) {
			return;
		}
		d_last = t;
		auto now = Time::Now();
		auto computationTime = now.Sub(d_lastShown);
		auto reminder = d_end.Sub(t);
		d_lastShown = now;
		Rcpp::Rcout << "Processed frame at " << t
		            << ", computed "
		            << ellapsed
		            << " in "  << computationTime
		            << ". Remind " << reminder
		            << ", ETA " << Duration((reminder.Seconds() / ellapsed.Seconds()) * computationTime.Seconds() * 1e9)
		            << "\n";
	}
};


Rcpp::List fmQueryIdentifyFrames(const fort::myrmidon::CExperiment & experiment,
                                 const fort::myrmidon::Time::ConstPtr & startTime,
                                 const fort::myrmidon::Time::ConstPtr & endTime,
                                 bool computeZones = false,
                                 bool singleThread = false,
                                 bool showProgress = false) {
	Time fstart;
	if ( showProgress == true ) {
		fstart = Time::Now();
	}
	std::vector<SEXP> res;
	size_t n = 0;
	std::function<void (const Time & t)> pd = [](const Time & t) {};
	if ( showProgress == true ) {
		ProgressDisplayer pdObj(experiment,startTime,endTime);
		pd = [pdObj](const Time & t) mutable -> void {
			     pdObj.ShowProgress(t);
		     };
	}
	Query::IdentifyFramesFunctor(experiment,
	                             [&res,pd](const IdentifiedFrame::ConstPtr & frame) {
		                             res.push_back(fmIdentifiedFrame_asR(frame));
		                             pd(frame->FrameTime);
	                             },
	                             startTime,
	                             endTime,
	                             computeZones,
	                             true);

	if ( showProgress == true ) {
		Rcpp::Rcout << "Processing took " << Time::Now().Sub(fstart) << "\n";
	}

	return Rcpp::List(res.cbegin(),res.cend());
}





RCPP_MODULE(queries) {

	Rcpp::function("fmIdentifiedFrameDebug",&fmIdentifiedFrame_debug);
	Rcpp::function("fmCollisionDebug",&fmCollision_debug);
	Rcpp::function("fmCollisionFrameDebug",&fmCollisionFrame_debug);
	Rcpp::function("fmAntTrajectoryDebug",&fmAntTrajectory_debug);
	Rcpp::function("fmAntInteractionDebug",&fmAntInteraction_debug);


	Rcpp::function("fmQueryComputeMeasurementFor",&fmQuery_computeMeasurementFor);
	Rcpp::function("fmQueryComputeTagStatistics",&fmQuery_computeTagStatistics);
	Rcpp::function("fmQueryIdentifyFrames",&fmQueryIdentifyFrames);
	//	Rcpp::function("fmQueryCollideFrames",&fmQuery_collideFrames);
	//Rcpp::function("fmQueryComputeTrajectories",&fmQuery_computeTrajectories);
	//Rcpp::function("fmQueryComputeAntInteractions",&fmQuery_computeAntInteractions);

}
