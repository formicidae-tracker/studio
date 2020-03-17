#include "KDTreeUTest.hpp"

#include "KDTreePrinter.hpp"
#include <opencv2/imgcodecs.hpp>

#include <fort/myrmidon/utils/FileSystem.hpp>

#include <random>

namespace fort {
namespace myrmidon {
namespace priv {

std::vector<KDTreeUTest::KDT::Element> KDTreeUTest::elements;
std::set<std::pair<int,int>> KDTreeUTest::collisions;
Duration KDTreeUTest::N2Duration(0);

void KDTreeUTest::SetUpTestSuite() {
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> xdist(0, 1920);
	std::uniform_int_distribution<int> ydist(0, 1080);

	std::uniform_int_distribution<int> bound(80, 100);
	const static size_t NUMBER_OF_OBJECT = 1000;

	elements.reserve(NUMBER_OF_OBJECT);

	for (size_t i = 0; i < NUMBER_OF_OBJECT; ++i) {
		Eigen::Vector2d min(xdist(e1),ydist(e1));
		Eigen::Vector2d max(min + Eigen::Vector2d(bound(e1),bound(e1)));
		auto volume = KDT::AABB(min,max);
		elements.push_back({int(i),volume});
	}

	// Do a Naive O(n2) to compute collisions
	auto start = Time::Now();
	for ( size_t i = 0; i < NUMBER_OF_OBJECT; ++i) {
		for ( size_t j = i+1; j < NUMBER_OF_OBJECT; ++j) {
			if ( elements[i].Volume.intersects(elements[j].Volume)) {
				collisions.insert(std::make_pair(i,j));
			}
		}
	}
	auto end = Time::Now();
	N2Duration = end.Sub(start);
}

void KDTreeUTest::TearDownTestSuite() {
	elements.clear();
	collisions.clear();
}


TEST_F(KDTreeUTest,TestCollision) {
	std::multiset<std::pair<int,int>> newCollisions;
	auto inserter = std::inserter(newCollisions,newCollisions.begin());
	auto start = Time::Now();
	auto kdtree = KDT::Build(elements.begin(),elements.end(),-1);
	kdtree->ComputeCollisions(inserter);
	auto end = Time::Now();
	auto duration = end.Sub(start);
	std::cerr << "[----------] N2Duration: "
	          << N2Duration << " KDTreeDuration: " <<duration
	          << std::endl;
	std::cerr << "[----------] Collisions reduced from " << (elements.size() * elements.size() + 1) / 2
	          << " to " << collisions.size() << std::endl;
	EXPECT_EQ(collisions.size(),newCollisions.size());
	for ( const auto & c : collisions ) {
		EXPECT_EQ(newCollisions.count(c), 1) << c.first << " and " << c.second << " should collide";
	}

	cv::Mat debug;
	KDTreePrinter::Print(debug,kdtree);
	cv::imwrite((fs::temp_directory_path() / "myrmidon-kdtree-test.png").c_str(),debug);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
