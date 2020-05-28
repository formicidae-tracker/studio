#include "queries.h"

#include "time.h"

#include <Rcpp.h>

struct fmIdentifiedFrame {
	fort::myrmidon::Time    FrameTime;
	fort::myrmidon::SpaceID Space;
	size_t                  Width;
	size_t                  Height;
	Rcpp::DataFrame         DataFrame;
	fmIdentifiedFrame(const fort::myrmidon::IdentifiedFrame::ConstPtr & frame )
		: FrameTime(frame->FrameTime)
		, Space(frame->Space)
		, Width(frame->Width)
		, Height(frame->Height) {

		size_t size = frame->Positions.size();
		Rcpp::IntegerVector IDs(size);
		Rcpp::NumericVector X(size),Y(size),Angle(size);
		size_t i = 0;
		for ( const auto & ant : frame->Positions ) {
			IDs[i] = ant.ID;
			X[i] = ant.Position.x();
			Y[i] = ant.Position.y();
			Angle[i] = ant.Angle;
			++i;
		}

		if ( frame->Zones.empty() == true ) {
			DataFrame = Rcpp::DataFrame::create( Rcpp::Named("AntID") = IDs,
			                                     Rcpp::_["X"] = X,
			                                     Rcpp::_["Y"] = Y,
			                                     Rcpp::_["Angle"] = Angle);
		} else  {
			Rcpp::IntegerVector Zones(size);
			i = 0;
			for ( const auto & z : frame->Zones ) {
				Zones[i] = z;
				++i;
			}
			DataFrame = Rcpp::DataFrame::create( Rcpp::Named("AntID") = IDs,
			                                     Rcpp::_["X"] = X,
			                                     Rcpp::_["Y"] = Y,
			                                     Rcpp::_["Angle"] = Angle,
			                                     Rcpp::_["ZoneID"] = Zones);
		}
	}

	void Show() const {
		Rcpp::Rcout << "fmIdentifiedFrame (\n"
		            << "  frameTime = " << FrameTime << "\n"
		            << "  space = " << Space << "\n"
		            << "  width = " << Width << "\n"
		            << "  height = " << Height << "\n"
		            << "  data = ";
		Rcpp::Function("str")(DataFrame);
		Rcpp::Rcout << ")\n";

	}

};

fmIdentifiedFrame fmIdentifiedFrame_debug() {
	using namespace fort::myrmidon;
	auto f = std::make_shared<IdentifiedFrame>();
	f->FrameTime = Time::Now();
	f->Space = 42;
	f->Width = 1920;
	f->Height = 1080;
	f->Positions.push_back(PositionedAnt{Eigen::Vector2d(0,0),0,1});
	f->Positions.push_back(PositionedAnt{Eigen::Vector2d(1,1),-0.1,2});
	f->Zones.push_back(0);
	f->Zones.push_back(2);

	return fmIdentifiedFrame(f);
}

RCPP_EXPOSED_CLASS(fmIdentifiedFrame);

struct fmCollision {
	fort::myrmidon::AntID  Ant1;
	fort::myrmidon::AntID  Ant2;
	fort::myrmidon::ZoneID Zone;
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

	fmCollision(const fort::myrmidon::Collision & c)
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

RCPP_EXPOSED_CLASS(fmCollision);

fort::myrmidon::Collision Collision_debug() {
	return fort::myrmidon::Collision{
		.IDs = std::make_pair(3,4),.InteractionTypes = {{1,1},{2,1},{1,3}},.Zone= 51};
}

fmCollision fmCollision_debug() {
	return fmCollision(Collision_debug());
}


struct fmCollisionFrame {
	fort::myrmidon::Time    FrameTime;
	fort::myrmidon::SpaceID Space;
	Rcpp::List              Collisions;
	void Show() const {
		Rcpp::Rcout << "fmCollisionFrame (\n"
		            << "  frameTime = " << FrameTime << "\n"
		            << "  space = " << Space << "\n"
		            << "  collisions = ";
		Rcpp::Function("str")(Collisions);
		Rcpp::Rcout << ")\n";
	}

	fmCollisionFrame(const fort::myrmidon::CollisionFrame::ConstPtr & f)
		: FrameTime(f->FrameTime)
		, Space(f->Space)
		, Collisions(f->Collisions.size()) {
		for( size_t i = 0; i < f->Collisions.size(); ++i ) {
			Collisions[i] = fmCollision(f->Collisions[i]);
		}
	}

};

RCPP_EXPOSED_CLASS(fmCollisionFrame);

fmCollisionFrame fmCollisionFrame_debug() {
	using namespace fort::myrmidon;
	auto f = std::make_shared<CollisionFrame>();
	f->FrameTime = Time::Now();
	f->Space = 42;
	for ( size_t i = 0 ; i < 3; ++i) {
		f->Collisions.push_back(Collision_debug());
	}
	return fmCollisionFrame(f);
}

struct fmAntTrajectory {
	fort::myrmidon::AntID   Ant;
	fort::myrmidon::SpaceID Space;
	fort::myrmidon::Time    Start;
	Rcpp::DataFrame         Data;

	void Show() const {
		Rcpp::Rcout << "fmAntTrajectory ("
		            << "  ant = " << Ant << "\n"
		            << "  space = " << Space << "\n"
		            << "  start = " << Start << "\n"
		            << "  data = ";
		Rcpp::Function("str")(Data);
		Rcpp::Rcout << ")\n";
	}


	fmAntTrajectory(const fort::myrmidon::AntTrajectory::ConstPtr & traj )
		: Ant(traj->Ant)
		, Space(traj->Space)
		, Start(traj->Start) {
		size_t n = traj->Seconds.size();
#define numericVectorFromEigen(Var,matrix,col,size) Rcpp::NumericVector Var(&((matrix)(0,col)),&((matrix)(0,col))+size)
		Rcpp::NumericVector Time(traj->Seconds.cbegin(),traj->Seconds.end());
		numericVectorFromEigen(X,traj->Positions,0,n);
		numericVectorFromEigen(Y,traj->Positions,1,n);
		numericVectorFromEigen(Angle,traj->Positions,2,n);
		if ( traj->Zones.empty() == true ) {
			Data = Rcpp::DataFrame::create(Rcpp::Named("Time (s)") = Time,
			                               Rcpp::_["X"] = X,
			                               Rcpp::_["Y"] = Y,
			                               Rcpp::_["Angle"] = Angle);
		} else {
			Rcpp::IntegerVector Zone(traj->Zones.begin(),traj->Zones.end());
			Data = Rcpp::DataFrame::create(Rcpp::Named("Time (s)") = Time,
			                               Rcpp::_["X"] = X,
			                               Rcpp::_["Y"] = Y,
			                               Rcpp::_["Angle"] = Angle,
			                               Rcpp::_["ZoneID"] = Zone);
		}
	};
};

RCPP_EXPOSED_CLASS(fmAntTrajectory);

fort::myrmidon::AntTrajectory::ConstPtr AntTrajectory_debug() {
	using namespace fort::myrmidon;
	auto res = std::make_shared<AntTrajectory>();
	res->Ant = 3;
	res->Space = 42;
	res->Start = Time::Now();
	res->Positions.resize(10,3);
	for ( size_t i = 0; i < 10 ; ++i ) {
		res->Seconds.push_back(i*0.1);
		res->Positions(i,0) = 1.0 * i;
		res->Positions(i,1) = -1.0 * i;
		res->Positions(i,2) = i * 0.1;
		res->Zones.push_back(i > 4 ? 51 : 0);
	}
	return res;
}

fmAntTrajectory fmAntTrajectory_debug() {
	return fmAntTrajectory(AntTrajectory_debug());
}

struct fmAntInteraction {
	fort::myrmidon::AntID  Ant1;
	fort::myrmidon::AntID  Ant2;
	Rcpp::List             Types;
	fmAntTrajectory        Ant1Trajectory,Ant2Trajectory;
	fort::myrmidon::Time   Start,End;

	void Show() const  {
		Rcpp::Function structure("str");
		Rcpp::Rcout << "fmAntInteraction (\n"
		            << "  ant1 = " << Ant1 << "\n"
		            << "  ant2 = " << Ant2 << "\n"
		            << "  start = " << Start << "\n"
		            << "  end = " << End << "\n"
		            << "  types = ";
		structure(Types);
		Rcpp::Rcout << "  ant1Trajectory = ";
		Ant1Trajectory.Show();
		Rcpp::Rcout << "  ant2Trajectory = ";
		Ant2Trajectory.Show();
		Rcpp::Rcout << ")\n";
	}

	fmAntInteraction(const fort::myrmidon::AntInteraction::ConstPtr & ai)
		: Ant1(ai->IDs.first)
		, Ant2(ai->IDs.second)
		, Types(ai->Types.size())
		, Ant1Trajectory(ai->Trajectories.first)
		, Ant2Trajectory(ai->Trajectories.second)
		, Start(ai->Start)
		, End(ai->End) {
		for ( size_t i = 0 ; i < ai->Types.size(); ++i ) {
			const auto & t = ai->Types[i];
			Types[i] = Rcpp::IntegerVector({(int)t.first,(int)t.second});
		}
	}
};

RCPP_EXPOSED_CLASS(fmAntInteraction);

fort::myrmidon::AntInteraction::ConstPtr AntInteraction_debug() {
	using namespace fort::myrmidon;
	auto res = std::make_shared<AntInteraction>();
	res->IDs = {3,4};
	res->Types = { {1,2},{1,3},{2,1} };
	res->Trajectories = {AntTrajectory_debug(),AntTrajectory_debug()};
	res->Start = Time::Now();
	res->End = res->Start.Add(10 * Duration::Second);
	return res;
}

fmAntInteraction fmAntInteraction_debug() {
	return fmAntInteraction(AntInteraction_debug());
}


RCPP_MODULE(queries) {
	Rcpp::class_<fmIdentifiedFrame>("fmIdentifiedFrame")
		.const_method("show",&fmIdentifiedFrame::Show)
		.field_readonly("frameTime",&fmIdentifiedFrame::FrameTime)
		.field_readonly("space",&fmIdentifiedFrame::Space)
		.field_readonly("width",&fmIdentifiedFrame::Width)
		.field_readonly("height",&fmIdentifiedFrame::Height)
		.field_readonly("data",&fmIdentifiedFrame::DataFrame)
		;

	Rcpp::class_<fmCollision>("fmCollision")
		.const_method("show",&fmCollision::Show)
		.field_readonly("ant1",&fmCollision::Ant1)
		.field_readonly("ant2",&fmCollision::Ant2)
		.field_readonly("zone",&fmCollision::Zone)
		.field_readonly("interactionTypes",&fmCollision::InteractionTypes)
		;

	Rcpp::class_<fmCollisionFrame>("fmCollisionFrame")
		.const_method("show",&fmCollisionFrame::Show)
		.field_readonly("frameTime",&fmCollisionFrame::FrameTime)
		.field_readonly("space",&fmCollisionFrame::Space)
		.field_readonly("collisions",&fmCollisionFrame::Collisions)
		;

	Rcpp::class_<fmAntTrajectory>("fmAntTrajectory")
		.const_method("show",&fmAntTrajectory::Show)
		.field_readonly("ant",&fmAntTrajectory::Ant)
		.field_readonly("space",&fmAntTrajectory::Space)
		.field_readonly("start",&fmAntTrajectory::Start)
		.field_readonly("data",&fmAntTrajectory::Data)
		;

	Rcpp::class_<fmAntInteraction>("fmAntInteraction")
		.const_method("show",&fmAntInteraction::Show)
		.field_readonly("ant1",&fmAntInteraction::Ant1)
		.field_readonly("ant2",&fmAntInteraction::Ant2)
		.field_readonly("ant1Trajectory",&fmAntInteraction::Ant1Trajectory)
		.field_readonly("ant2Trajectory",&fmAntInteraction::Ant2Trajectory)
		.field_readonly("types",&fmAntInteraction::Types)
		.field_readonly("start",&fmAntInteraction::Start)
		.field_readonly("end",&fmAntInteraction::End)
		;

	Rcpp::function("fmIdentifiedFrameDebug",&fmIdentifiedFrame_debug);
	Rcpp::function("fmCollisionDebug",&fmCollision_debug);
	Rcpp::function("fmCollisionFrameDebug",&fmCollisionFrame_debug);
	Rcpp::function("fmAntTrajectoryDebug",&fmAntTrajectory_debug);
	Rcpp::function("fmAntInteractionDebug",&fmAntInteraction_debug);

}
