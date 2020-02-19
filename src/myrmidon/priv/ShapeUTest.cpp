#include "ShapeUTest.hpp"

#include "Shape.hpp"
#include <myrmidon/Time.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(ShapeUTest,TestCaspuleCollision) {
	struct TestData {
		double aC1X,aC1Y,aC2X,aC2Y,aR1,aR2;
		double bC1X,bC1Y,bC2X,bC2Y,bR1,bR2;
		bool Expected;
	};


	std::vector<TestData> testdata =
		{
		 //Toy example with square positions
		 {
		  0,0,0,1,0.25,0.25,
		  1,0,1,1,0.25,0.25,
		  false,
		 },
		 {
		  0,0,0,1,0.6,0.6,
		  1,0,1,1,0.6,0.6,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.35,0.55,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.55,0.35,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.35,0.55,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.55,0.35,
		  true,
		 },
		 // More complicated example, where intersection is not on the
		 // minimal distance between segments
		 {
		  0,0,0,1,0.3,0.7,
		  1,0.1,1.2,1.2,0.3,0.7,
		  true,
		 },
		 // very special case that show up if the radii interpolation
		 // are wrong
		 {
		  0,0,0,1,0.01,0.1,
		  1,0.1,1.1,0.9,0.01,1.2,
		  false,
		 },
		};

	std::vector<std::tuple<Duration,Duration,Capsule,Capsule> > durations;
	size_t i = 0;
	auto gStart = Time::Now();
	for ( ; i < (500 * 501) / 2; ++i) {
		size_t ii = 0;
		for ( const auto & d : testdata ) {
			Capsule a(Eigen::Vector2d(d.aC1X,d.aC1Y),
			          Eigen::Vector2d(d.aC2X,d.aC2Y),
			          d.aR1,
			          d.aR2);

			Capsule b(Eigen::Vector2d(d.bC1X,d.bC1Y),
			          Eigen::Vector2d(d.bC2X,d.bC2Y),
			          d.bR1,
			          d.bR2);

			auto start = Time::Now();
			bool res = Capsule::Intersect(a.C1(),a.C2(),a.R1(),a.R2(),
			                              b.C1(),b.C2(),b.R1(),b.R2());
			auto end = Time::Now();
			EXPECT_EQ(res,d.Expected) << " Intersecting " << a << " and " << b;
			auto dur = end.Sub(start);
			if ( i == 0 ) {
				durations.push_back({dur,dur,a,b});
			} else {
				std::get<1>(durations[ii]) = std::get<1>(durations[ii]) + dur;
			}
			++ii;
		}
	}
	auto gEnd = Time::Now();
	for ( const auto & b : durations ) {
		std::cerr << "Collision detection between " << std::get<2>(b)
		          << " and " << std::get<3>(b)
		          << " took first:" << std::get<0>(b)
		          << " mean:" << Duration(std::get<1>(b).Nanoseconds() / i)
		          << std::endl;
	}
	std::cerr << "Total time for " << i*testdata.size() << " detections: " << gEnd.Sub(gStart) << std::endl;


}

} // namespace priv
} // namespace myrmidon
} // namespace fort
