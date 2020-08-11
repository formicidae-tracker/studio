#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Query.hpp>


#include <Rcpp.h>



std::tuple<Rcpp::IntegerVector,Rcpp::NumericVector,Rcpp::NumericVector,Rcpp::NumericVector>
fmPositionData_asR(const fort::myrmidon::PositionedAntList & ants);

SEXP fmIdentifiedFrames_asR(std::vector<fort::myrmidon::IdentifiedFrame::ConstPtr> & frames);

SEXP fmIdentifiedFramesWithZones_asR(std::vector<fort::myrmidon::IdentifiedFrame::ConstPtr> & frames);

std::string fmInteractionTypes_asStr(const fort::myrmidon::InteractionTypes & types );

SEXP fmCollisionData_asR(std::vector<fort::myrmidon::Query::CollisionData> & frames );

Rcpp::DataFrame fmAntTrajectoryPosition_asR(const fort::myrmidon::AntTrajectory::ConstPtr & trajectory);

std::tuple<Rcpp::DataFrame,Rcpp::List>
fmAntTrajectories_asR(const std::vector<fort::myrmidon::AntTrajectory::ConstPtr> & trajectories);

struct TrajectoryIndexing {
	std::vector<size_t> RowIndexes,SubRowStart,SubRowEnd;
	void Push(const fort::myrmidon::AntTrajectorySegment & segment);
};

struct TrajectorySummary {
	std::vector<double>      Means;
	std::vector<std::string> Zones;
	void Push(const Eigen::Vector3d & mean,
	          const std::string & zone);
};

Rcpp::DataFrame
fmAntInteractions_asR(std::vector<fort::myrmidon::AntInteraction::ConstPtr> & interactions,
                      const TrajectoryIndexing & ant1Indexing,
                      const TrajectoryIndexing & ant2Indexing);

Rcpp::DataFrame
fmAntInteractionsSummarized_asR(std::vector<fort::myrmidon::AntInteraction::ConstPtr> & interactions,
                                TrajectorySummary & ant1Summary,
                                TrajectorySummary & ant2Summary);
