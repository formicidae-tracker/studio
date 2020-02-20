#include <random>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <myrmidon/priv/KDTreePrinter.hpp>
#include <myrmidon/Time.hpp>
#include <myrmidon/utils/FileSystem.hpp>
#include <myrmidon/priv/Shape.hpp>
#include <myrmidon/priv/KDTree.hpp>




namespace fort {
namespace myrmidon {
namespace priv {

typedef KDTree<int,double,2> KDT;

void BuildElements(std::vector<KDT::Element> & elements,
                   size_t number,
                   size_t width,
                   size_t height,
                   size_t minSize,
                   size_t maxSize) {
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> xdist(0, width);
	std::uniform_int_distribution<int> ydist(0, height);

	std::uniform_int_distribution<int> bound(minSize, maxSize);
	elements.reserve(number);
	elements.clear();
	for ( size_t i = 0; i < number ; ++i ) {
		Eigen::Vector2d min(xdist(e1),ydist(e1));
		Eigen::Vector2d max(min + Eigen::Vector2d(bound(e1),bound(e1)));
		elements.push_back({int(i),KDT::AABB(min,max)});
	}
}



void BenchmarkKDTree( const fs::path &  result) {
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

			std::vector<KDT::Element> elements;
			BuildElements(elements,b.Number,1920,1080,80,100);

			auto start = Time::Now();
			auto kdtree = KDT::Build(elements.begin(),elements.end(),b.Depth);
			auto end = Time::Now();
			b.ExecTime.push_back(end.Sub(start));
			auto els = kdtree->ElementSeparation();
			b.NBLower.push_back(els.first);
			b.NBUpper.push_back(els.second);
		};
	std::ofstream out(result.c_str());
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

}
}
}

namespace fmp = fort::myrmidon::priv;

void Execute(int argc, char ** argv) {
	if ( argc != 2 ) {
		throw std::invalid_argument("Need a directory to save the benchmark results");
	}
	fs::path dirpath(argv[1]);
	if ( fs::is_directory(dirpath) == false ) {
		throw std::invalid_argument(dirpath.string() + " is not a directory");
	}

	fmp::BenchmarkKDTree(dirpath / "benchmark_kdtree.txt");

	std::vector<fmp::KDT::Element> elements;
	fmp::BuildElements(elements,20,1920,1080,80,100);
	auto kdt = fmp::KDT::Build(elements.begin(),elements.end(),-1);
	cv::Mat toPrint;
	fmp::KDTreePrinter::Print(toPrint,kdt);
	cv::imwrite((dirpath / "example_kdtree.png").c_str(),toPrint);

}


int main(int argc, char ** argv) {
	try {
		Execute(argc,argv);
	} catch (const std::exception & e) {
		std::cerr << "Got uncaught exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
