#include "types.h"

#include "time.h"

using namespace fort::myrmidon;

#include <Rcpp.h>


void TrajectorySummary::Push(const Eigen::Vector3d & mean,
                             const std::string & zone) {
	Means.push_back(mean.x());
	Means.push_back(mean.y());
	Means.push_back(mean.z());
	Zones.push_back(zone);
}


void TrajectoryIndexing::Push(const fort::myrmidon::AntTrajectorySegment & segment) {
	RowIndexes.push_back(0);
	// R is 1 indexed and last index includes range.
	SubRowStart.push_back(segment.Begin+1);
	SubRowEnd.push_back(segment.End);
}


#define Vectorize(...) {__VA_ARGS__}
#define CreateDataFrameOptimized(VarName,elements,names,nrows ) \
	Rcpp::List VarName(elements); \
	VarName.attr("names") = Rcpp::CharacterVector(names); \
	VarName.attr("row.names") = Rcpp::IntegerVector::create(NA_INTEGER,(nrows)); \
	VarName.attr("class") = "data.frame"


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



SEXP fmIdentifiedFrames_asR_private(std::vector<IdentifiedFrame::ConstPtr> & frames, bool withZone) {
	using namespace Rcpp;
	List positions(frames.size());
	DatetimeVector time(frames.size());
	IntegerVector width(frames.size()),height(frames.size()),space(frames.size());
	size_t i = 0;
	for ( auto & f : frames ) {
		auto [IDs,X,Y,Angle] = fmPositionData_asR(f->Positions);
		if ( withZone == true ) {
			CreateDataFrameOptimized(df,
			                         Vectorize(IDs,X,Y,Angle, IntegerVector(f->Zones.begin(),f->Zones.end())),
			                         Vectorize("antID","x","y","angle","zone"),
			                         IDs.size());

			positions[i] = df;
		} else {
			CreateDataFrameOptimized(df,
			                         Vectorize(IDs,X,Y,Angle),
			                         Vectorize("antID","x","y","angle"),
			                         IDs.size());
			positions[i] = df;
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

SEXP fmIdentifiedFramesWithZones_asR(std::vector<IdentifiedFrame::ConstPtr> & frames) {
	return fmIdentifiedFrames_asR_private(frames,true);
}

SEXP fmIdentifiedFrames_asR(std::vector<IdentifiedFrame::ConstPtr> & frames) {
	return fmIdentifiedFrames_asR_private(frames,false);
}


std::string fmInteractionTypes_asStr(const InteractionTypes & types ) {
	std::ostringstream res;
	for ( size_t i = 0; i < types.rows(); ++i ) {
		res << types(i,0) <<  "-" <<  types(i,1) << ",";
	}
	auto ress = res.str();
	ress.pop_back();
	return ress;
}

SEXP fmCollisionData_asR(std::vector<Query::CollisionData> & frames ) {
	using namespace Rcpp;
	List positions(frames.size());
	DatetimeVector time(frames.size());
	IntegerVector width(frames.size()),height(frames.size()),space(frames.size());

	std::vector<std::string> types;
	std::vector<AntID> ant1IDs,ant2IDs;
	std::vector<size_t> frameRowIndex;
	std::vector<ZoneID> zone;
	size_t i = 0;
	for ( auto & [identified,collided] : frames) {
		auto [IDs,X,Y,Angle] = fmPositionData_asR(identified->Positions);
		CreateDataFrameOptimized(df,
		                         Vectorize(IDs,X,Y,Angle, IntegerVector(identified->Zones.begin(),identified->Zones.end())),
		                         Vectorize("antID","x","y","angle","zone"),
		                         IDs.size());

		positions[i] = df;

		time[i] = fmTime_asR(identified->FrameTime);
		width[i] = identified->Width;
		height[i] = identified->Height;
		space[i] = identified->Space;

		for ( const auto & collision : collided->Collisions ) {
			frameRowIndex.push_back(i+1); // R index starts at 1, not 0.
			ant1IDs.push_back(collision.IDs.first);
			ant2IDs.push_back(collision.IDs.second);
			zone.push_back(collision.Zone);
			types.push_back(fmInteractionTypes_asStr(collision.Types));
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
	                                                        _["types"] = CharacterVector(types.cbegin(),types.cend()),
	                                                        _["frames_row_index"] = IntegerVector(frameRowIndex.begin(),
		                     frameRowIndex.end())));
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
		CreateDataFrameOptimized(df,
		                         Vectorize(time,x,y,angle, IntegerVector(trajectory->Zones.cbegin(),trajectory->Zones.cend())),
		                         Vectorize("time","x","y","angle","zone"),
		                         trajectory->Positions.rows());
		return df;
	}
	CreateDataFrameOptimized(df,
	                         Vectorize(time,x,y,angle),
	                         Vectorize("time","x","y","angle"),
	                         trajectory->Positions.rows());

	return df;
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

Rcpp::DataFrame
fmAntInteractions_asR(std::vector<AntInteraction::ConstPtr> & interactions,
                      const TrajectoryIndexing & ant1Indexing,
                      const TrajectoryIndexing & ant2Indexing) {
	using namespace Rcpp;
	auto n = interactions.size();
	List ant1Trajectory(n),ant2Trajectory(n);
	IntegerVector ant1(n),ant2(n),space(n);
	DatetimeVector start(n),end(n);
	CharacterVector types(n);

#define assert_size(v,s) do{	  \
		if ( (v).size() != (s) ) { \
			throw std::runtime_error("Invalid vector size for " #v " ( got:" +  std::to_string(v.size()) + " expected:" +std::to_string(s)); \
		} \
	} while(0)


	size_t i = 0;
	for ( auto & interaction : interactions ) {
		ant1[i] = interaction->IDs.first;
		ant2[i] = interaction->IDs.second;
		start[i] = fmTime_asR(interaction->Start);
		end[i] = fmTime_asR(interaction->End);
		space[i] = interaction->Space;

		types[i] = fmInteractionTypes_asStr(interaction->Types);

		++i;
		interaction.reset();
	}


	return DataFrame::create(_["ant1"] = ant1,
	                         _["ant2"] = ant2,
	                         _["start"] = start,
	                         _["end"] = end,
	                         _["space"] = space,
	                         _["types"]  = types,
	                         _["ant1.trajectory.row"]  = IntegerVector(ant1Indexing.RowIndexes.cbegin(),
	                                                                   ant1Indexing.RowIndexes.cend()),
	                         _["ant1.trajectory.start"]  = IntegerVector(ant1Indexing.SubRowStart.cbegin(),
	                                                                     ant1Indexing.SubRowStart.cend()),
	                         _["ant1.trajectory.end"]  = IntegerVector(ant1Indexing.SubRowEnd.cbegin(),
	                                                                   ant1Indexing.SubRowEnd.cend()),
	                         _["ant2.trajectory.row"]  = IntegerVector(ant2Indexing.RowIndexes.cbegin(),
	                                                                   ant2Indexing.RowIndexes.cend()),
	                         _["ant2.trajectory.start"]  = IntegerVector(ant2Indexing.SubRowStart.cbegin(),
	                                                                     ant2Indexing.SubRowStart.cend()),
	                         _["ant2.trajectory.end"]  = IntegerVector(ant2Indexing.SubRowEnd.cbegin(),
	                                                                   ant2Indexing.SubRowEnd.cend())
	                         );


}


Rcpp::DataFrame
fmAntInteractionsSummarized_asR(std::vector<AntInteraction::ConstPtr> & interactions,
                                TrajectorySummary & ant1Summary,
                                TrajectorySummary & ant2Summary) {
	using namespace Rcpp;
	auto n = interactions.size();

	IntegerVector ant1(n),ant2(n),space(n);
	DatetimeVector start(n),end(n);
	CharacterVector types(n);

	size_t i = 0;
	for ( auto & interaction : interactions) {
		ant1[i] = interaction->IDs.first;
		ant2[i] = interaction->IDs.second;
		start[i] = fmTime_asR(interaction->Start);
		end[i] = fmTime_asR(interaction->End);
		space[i] = interaction->Space;

		types[i] = fmInteractionTypes_asStr(interaction->Types);

		++i;
		interaction.reset();
	}

	//first go from row-major to column-major using eigen.
	Eigen::MatrixXd mean1 = Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&ant1Summary.Means[0],n,3);
	ant1Summary.Means.clear();
	Eigen::MatrixXd mean2 = Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,3,Eigen::RowMajor>>(&ant2Summary.Means[0],n,3);
	ant2Summary.Means.clear();

	// now duplicates again the data to R, from column major order
	numericVectorFromEigen(x1mean,mean1,0,n);
	numericVectorFromEigen(y1mean,mean1,1,n);
	numericVectorFromEigen(angle1mean,mean1,2,n);

	numericVectorFromEigen(x2mean,mean2,0,n);
	numericVectorFromEigen(y2mean,mean2,1,n);
	numericVectorFromEigen(angle2mean,mean2,2,n);

	// now we can put everything in data.frame
	return DataFrame::create(_["ant1"] = ant1,
	                         _["ant2"] = ant2,
	                         _["start"] = start,
	                         _["end"] = end,
	                         _["space"] = space,
	                         _["types"] = types,
	                         _["ant1.mean.x"] = x1mean,
	                         _["ant1.mean.y"] = y1mean,
	                         _["ant1.mean.angle"] = angle1mean,
	                         _["ant2.mean.x"] = x2mean,
	                         _["ant2.mean.y"] = y2mean,
	                         _["ant2.mean.angle"] = angle2mean,
	                         _["ant1.zones"] = CharacterVector(ant1Summary.Zones.cbegin(),ant1Summary.Zones.cend()),
	                         _["ant2.zones"] = CharacterVector(ant2Summary.Zones.cbegin(),ant2Summary.Zones.cend()));
}
