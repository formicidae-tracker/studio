#include "ShapeUTest.hpp"

#include <random>
#include <fstream>


#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


#include "Shape.hpp"
#include <myrmidon/Time.hpp>
#include "KDTree.hpp"

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
	for ( ; i < (10 * 11) / 2; ++i) {
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




void PrintKDTree(cv::Mat & result,
                 const typename KDTree<int,double,2>::ConstPtr & tree) {
	typedef KDTree<int,double,2> KDT;
	typedef typename KDTree<int,double,2>::Node::Ptr NodePtr;
	auto offset = tree->d_root->Volume.min() - Eigen::Vector2d(20,20);
	auto size = tree->d_root->Volume.max() - offset + Eigen::Vector2d(20,20);
	std::cerr << size.transpose() << std::endl;
	result = cv::Mat(size.y(),size.x(),CV_8UC3);
	result.setTo(cv::Vec3b(255,255,255));

	auto toCv = [offset](const Eigen::Vector2d & p) {
		            return cv::Point(p.x() - offset.x(),
		                             p.y() - offset.y());
	            };
	auto drawAABB =
		[&result,toCv](const typename KDT::AABB & volume,cv::Vec3b color, int thickness) {
			cv::line(result,
			         toCv(volume.min()),
			         toCv(Eigen::Vector2d(volume.min().x(),volume.max().y())),
			         color,
			         thickness);
			cv::line(result,
			         toCv(Eigen::Vector2d(volume.min().x(),volume.max().y())),
			         toCv(volume.max()),
			         color,
			         thickness);
			cv::line(result,
			         toCv(volume.max()),
			         toCv(Eigen::Vector2d(volume.max().x(),volume.min().y())),
			         color,
			         thickness);
			cv::line(result,
			         toCv(Eigen::Vector2d(volume.max().x(),volume.min().y())),
			         toCv(volume.min()),
			         color,
			         thickness);

		};

	std::vector<NodePtr> toProcess =
		{
		 tree->d_root,
		};
	std::vector<typename KDT::AABB> volumes =
		{
		 KDT::AABB(tree->d_root->Volume),
		};
	drawAABB(volumes[0],cv::Vec3b(0,0,0),4);
	for ( size_t i = 0 ; i < toProcess.size(); ++i) {
		auto n = toProcess[i];
		auto volume = volumes[i];
		auto center = (n->ObjectVolume.min() + n->ObjectVolume.max()) / 2;
		size_t dim = n->Depth % 2;
		if ( n->Lower ) {
			auto newVolume = volume;
			newVolume.max()(dim,0) = center(dim,0);
			toProcess.push_back(n->Lower);
			volumes.push_back(newVolume);
		}
		if ( n->Upper ) {
			auto newVolume = volume;
			newVolume.min()(dim,0) = center(dim,0);
			toProcess.push_back(n->Upper);
			volumes.push_back(newVolume);
		}

		drawAABB(n->ObjectVolume,cv::Vec3b(255,255,0),2);
		drawAABB(n->Volume,cv::Vec3b(0,0,0),1);

		if ( dim == 0 ) {
			cv::line(result,
			         toCv(Eigen::Vector2d(center.x(),volume.min().y())),
			         toCv(Eigen::Vector2d(center.x(),volume.max().y())),
			         cv::Vec3b(0,0,255),
			         2);
		} else {
			cv::line(result,
			         toCv(Eigen::Vector2d(volume.min().x(),center.y())),
			         toCv(Eigen::Vector2d(volume.max().x(),center.y())),
			         cv::Vec3b(255,0,0),
			         2);
		}

		cv::circle(result,toCv(center),4,cv::Vec3b(0,0,0),-1);

	}
}




TEST_F(ShapeUTest,KDTree) {
	struct BenchmarkData {
		size_t                Number;
		int                   Depth;
		std::vector<Duration> ExecTime;
		std::vector<size_t>   NBLower;
		std::vector<size_t>   NBUpper;
	};
	std::vector<size_t> Numbers = {10,20,50,100,200,500,1000,2000,4000,6000,8000,10000};
	std::vector<int> Depths = {-1,0,1,2,3};

	std::vector<BenchmarkData> benchmarks;
	for ( const auto & d : Depths) {
		for ( const auto & n :Numbers ) {
			benchmarks.push_back({n,d,{},{},{}});
		}
	}

	auto performOne =
		[](BenchmarkData & b) {
			std::cerr << "N: " << b.Number << " Depth: " << b.Depth << std::endl;
			std::random_device r;
			std::default_random_engine e1(r());
			std::uniform_int_distribution<int> xdist(0, 1920);
			std::uniform_int_distribution<int> ydist(0, 1080);

			std::uniform_int_distribution<int> bound(80, 100);

			typedef KDTree<int,double,2> KDT;
			std::vector<KDT::Element> elements;
			for ( size_t i = 0; i < b.Number ; ++i ) {
				Eigen::Vector2d min(xdist(e1),ydist(e1));
				Eigen::Vector2d max(min + Eigen::Vector2d(bound(e1),bound(e1)));
				elements.push_back({int(i),KDT::AABB(min,max)});
			}
			auto start = Time::Now();
			auto kdtree = KDT::Build(elements.begin(),elements.end(),b.Depth);
			auto end = Time::Now();
			b.ExecTime.push_back(end.Sub(start));
			auto els = kdtree->ElementSeparation();
			b.NBLower.push_back(els.first);
			b.NBUpper.push_back(els.second);
		};
	std::ofstream out("/tmp/benchmark_kdtree.txt");
	out << "#Number,Depth,Time(us),Lower,Upper" << std::endl;
	for ( size_t i = 0; i < 100; ++i ) {
		for ( auto & b : benchmarks ) {
			performOne(b);
			out << b.Number << ","
			    << b.Depth << ","
			    << b.ExecTime.back().Microseconds() << ","
			    << b.NBLower.back() << ","
			    << b.NBUpper.back()
			    << std::endl;
		}
	}






}


} // namespace priv
} // namespace myrmidon
} // namespace fort
