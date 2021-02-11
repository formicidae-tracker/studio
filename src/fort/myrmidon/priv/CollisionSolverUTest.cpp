#include "CollisionSolverUTest.hpp"

#include <random>

#include "AntShapeType.hpp"
#include "AntMetadata.hpp"
#include "Capsule.hpp"
#include "Circle.hpp"
#include "Polygon.hpp"
#include "Space.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

IdentifiedFrame::Ptr     CollisionSolverUTest::frame;
Space::Universe::Ptr     CollisionSolverUTest::universe;
AntByID                  CollisionSolverUTest::ants;
CollisionFrame::ConstPtr CollisionSolverUTest::collisions;

void debugToImage(const IdentifiedFrame::ConstPtr & frame,
                  const CollisionFrame::ConstPtr & collision,
                  const AntByID & ants ) {
	cv::Mat debug(frame->Height/3,frame->Width/3,CV_8UC3);
	debug.setTo(cv::Scalar(255,255,255));

	std::map<int,cv::Point> converted;
	for ( const auto & p : frame->Positions ) {
		converted.insert(std::make_pair(p.ID,
		                                cv::Point(p.Position.x(),
		                                          p.Position.y())/3));
		auto ant = ants.at(p.ID);
		auto antIso = Isometry2Dd(p.Angle,p.Position);
		for ( const auto & [t,c] : ant->Capsules() ) {
			auto color = t == 1 ? cv::Scalar(0,255,0) : cv::Scalar(255,0,255);
			auto cc = c.Transform(antIso);
			auto center1 = cv::Point(cc.C1().x(),cc.C1().y())/3;
			auto center2 = cv::Point(cc.C2().x(),cc.C2().y())/3;
			cv::circle(debug,center1,c.R1()/3,color,2);
			cv::circle(debug,center2,c.R2()/3,color,2);
			cv::line(debug,center1,center2,color,2);
		}
	}

	for ( const auto & inter : collision->Collisions ) {
		const auto & a = converted.at(inter.IDs.first);
		const auto & b = converted.at(inter.IDs.second);
		cv::line(debug,a,b,cv::Scalar(0,0,255),4);
	}

	for ( const auto & [ID,pos] : converted) {
		cv::circle(debug,pos,10,cv::Scalar(255,0,0),-1);
		cv::putText(debug,
		            std::to_string(ID),
		            pos,
		            cv::FONT_HERSHEY_SIMPLEX,
		            1.0,
		            cv::Scalar(0,0,0));
	}

	cv::imwrite("debug.png",debug);
}



void CollisionSolverUTest::SetUpTestSuite() {
	std::random_device r;
	std::default_random_engine e1(r());

	const static int HEIGHT = 6004;
	const static int WIDTH = 7920;
	std::uniform_real_distribution<double> noise(-5.0, 5.0);
	std::uniform_real_distribution<double> xPos(0,WIDTH);
	std::uniform_real_distribution<double> yPos(0,HEIGHT);
	std::uniform_real_distribution<double> angle(-M_PI,M_PI);

	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	frame = identifiedFrame;
	identifiedFrame->Width = WIDTH;
	identifiedFrame->Height = HEIGHT;
	auto shapeTypes = std::make_shared<AntShapeTypeContainer>();
	auto metadata = std::make_shared<AntMetadata>();
	shapeTypes->Create("body",1);
	shapeTypes->Create("antennas",2);
	for ( size_t i = 0; i < 200; ++i ) {
		auto ant = std::make_shared<Ant>(shapeTypes,
		                                 metadata,
		                                 i+1);
		ant->AddCapsule(1,Capsule(Eigen::Vector2d(40+noise(e1),0),
		                          Eigen::Vector2d(-100+2*noise(e1),0),
		                          40+noise(e1),
		                          60+noise(e1)));
		ant->AddCapsule(2,Capsule(Eigen::Vector2d(20,10),
		                          Eigen::Vector2d(60,50),
		                          25,40));
		ant->AddCapsule(2,Capsule(Eigen::Vector2d(20,-10),
		                          Eigen::Vector2d(60,-50),
		                          25,40));

		ants.insert(std::make_pair(ant->AntID(),ant));

		identifiedFrame->Positions.push_back(PositionedAnt{.Position = Eigen::Vector2d(xPos(e1),yPos(e1)),
		                                                   .Angle = angle(e1),
		                                                   .ID = ant->AntID(),

			});
	}


	//defines the space
	universe = std::make_shared<Space::Universe>();
	auto foo = Space::Universe::Create(universe,1,"foo");
	identifiedFrame->Space = 1;
	auto nest = foo->CreateZone("nest");
	std::vector<Shape::ConstPtr> nestShapes = {std::make_shared<Polygon>(Vector2dList({{WIDTH/2,0},{WIDTH,0},{WIDTH,HEIGHT},{WIDTH/2,HEIGHT}}))};
	nest->AddDefinition(nestShapes,
	                    Time::SinceEver(),Time::Forever());

	auto food = foo->CreateZone("food");
	std::vector<Shape::ConstPtr> foodShapes = { std::make_shared<Circle>(Eigen::Vector2d(WIDTH/4,HEIGHT/2),
	                                                                    WIDTH/8),
	};
	food->AddDefinition(foodShapes,
	                    Time::SinceEver(),Time::Forever());

	collisions = NaiveCollisions();

	debugToImage(frame,collisions,ants);
}


CollisionFrame::ConstPtr CollisionSolverUTest::NaiveCollisions() {
	std::unordered_map<Zone::ID,std::vector<PositionedAnt> > locatedAnt;
	for ( const auto & p : frame->Positions ) {
		bool found =  false;
		for ( const auto & [zID,zone] : universe->Spaces().at(1)->Zones() ) {
			if ( zone->AtTime(Time())->Contains(p.Position) == true ) {
				locatedAnt[zID].push_back(p);
				found = true;
				break;
			}
		}
		if ( found == false ) {
			locatedAnt[0].push_back(p);
		}
	}

	auto collides =
		[](const PositionedAnt & a,
		   const PositionedAnt & b) {
			std::vector<std::pair<uint32_t,uint32_t>> res;
			auto aAnt = ants.at(a.ID);
			auto bAnt = ants.at(b.ID);
			Isometry2Dd aIso(a.Angle,a.Position);
			Isometry2Dd bIso(b.Angle,b.Position);
			for ( const auto & [aType,aC] : aAnt->Capsules() ) {
				Capsule aCapsule = aC.Transform(aIso);
				for ( const auto & [bType,bC] : bAnt->Capsules() ) {
					Capsule bCapsule = bC.Transform(bIso);
					if ( aCapsule.Intersects(bCapsule) == true ) {
						res.push_back(std::make_pair(aType,bType));
					}
				}
			}
			InteractionTypes asM(res.size(),2);
			for ( size_t i = 0; i < res.size(); ++i) {
				asM(i,0) = res[i].first;
				asM(i,1) = res[i].second;
			}

			return asM;
		};

	auto res = std::make_shared<CollisionFrame>();

	for ( const auto & [zID,ants] : locatedAnt ) {
		for ( size_t i = 0; i < ants.size(); ++i) {
			for ( size_t j = i+1; j < ants.size(); ++j ) {
				auto collisions = collides(ants[i],ants[j]);
				if ( collisions.rows() != 0 )  {
					res->Collisions.push_back({std::make_pair(ants[i].ID,ants[j].ID),
					                             collisions,
					                             zID});
				}
			}
		}
	}
	return res;
}


TEST_F(CollisionSolverUTest,TestE2E) {
	auto solver = std::make_shared<CollisionSolver>(universe->Spaces(),
	                                                  ants);
	CollisionFrame::ConstPtr res;
	EXPECT_THROW({
			frame->Space = 2;
			res = solver->ComputeCollisions(frame);
		},std::invalid_argument);
	EXPECT_NO_THROW({
			frame->Space = 1;
			res = solver->ComputeCollisions(frame);
		});
	for ( const auto & inter : collisions->Collisions ) {
		auto fi = std::find_if(res->Collisions.begin(),
		                       res->Collisions.end(),
		                       [&inter](const Collision & i) {
			                       return i.IDs == inter.IDs;
		                       });
		if ( fi == res->Collisions.end() ) {
			ADD_FAILURE() << "Missing collision between " << Ant::FormatID(inter.IDs.first)
			              << " and " << Ant::FormatID(inter.IDs.second);
			continue;
		}


		for ( size_t i = 0; i < inter.Types.rows(); ++i ) {
			bool good = false;
			for (size_t j = 0; j < fi->Types.rows(); ++j) {
				if ( inter.Types.row(i) == fi->Types.row(j) ) {
					good = true;
					break;
				}
			}
			if ( good == false ) {
				auto aName = inter.Types(i,0) == 1 ? "body" : "antennas";
				auto bName = inter.Types(i,1) == 1 ? "body" : "antennas";
				ADD_FAILURE() << "Collision between " << Ant::FormatID(inter.IDs.first)
				              << " and " << Ant::FormatID(inter.IDs.second)
				              << " is missing collision " << aName << "-" << bName;
			}
		}
	}
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
