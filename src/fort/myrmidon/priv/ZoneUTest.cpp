#include "ZoneUTest.hpp"


#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/priv/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


void ZoneUTest::SetUp() {
	zone = Zone::Create(1,"foo","bar");
	shapes
		= {
		   std::static_pointer_cast<const Shape>(std::make_shared<Circle>(Eigen::Vector2d(0,0),2.0)),
		   std::static_pointer_cast<const Shape>(std::make_shared<Polygon>(Vector2dList({{-1,-1},{1,-1},{1,1},{-1,1}}))),
	};

}

void ZoneUTest::TearDown() {
	zone.reset();
	shapes.clear();
}

TEST_F(ZoneUTest,GeometryHaveAABB) {

	Zone::Geometry g(shapes);
	EXPECT_TRUE(AABBAlmostEqual(g.GlobalAABB(),shapes.front()->ComputeAABB()));
	ASSERT_EQ(shapes.size(), g.IndividualAABB().size());
	for( int i = 0; i < shapes.size(); ++i) {
		EXPECT_TRUE(AABBAlmostEqual(shapes[i]->ComputeAABB(),
		                            g.IndividualAABB()[i]));
	}
}

TEST_F(ZoneUTest,DefinitionOwnsGeometry) {
	auto definition = zone->AddDefinition({},
	                                      Time::SinceEver(),
	                                      Time::Forever());

	// Even if we pass a nullptr, geometry is not null
	ASSERT_FALSE(!definition->GetGeometry());

	EXPECT_TRUE(definition->GetGeometry()->Shapes().empty());

	definition->SetGeometry(std::make_shared<Zone::Geometry>(shapes));
	ASSERT_EQ(definition->GetGeometry()->Shapes(),
	          shapes);
}

TEST_F(ZoneUTest,ZoneCanBeRenamed) {
	EXPECT_NO_THROW({
			zone->SetName("foo");
			zone->SetName("bar");
			zone->SetName("");
		});
}

TEST_F(ZoneUTest,DefinitionAreTimeValidObject) {

	auto start = Time::SinceEver();
	auto end = Time::Forever();
	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TRUE(TimeEqual(start,Time::SinceEver()));
	EXPECT_TRUE(TimeEqual(end,Time::Forever()));


	auto definition = zone->AddDefinition(shapes,Time::SinceEver(),Time::Forever());



	EXPECT_THROW({
			zone->AddDefinition(shapes,
			                    Time::FromTimeT(0),
			                    Time::Forever());
		},std::runtime_error);

	definition->SetStart(Time::FromTimeT(1));

	EXPECT_THROW({
			definition->SetEnd(Time::FromTimeT(0));
		},std::invalid_argument);

	EXPECT_NO_THROW({
			definition->SetEnd(Time::FromTimeT(2));
		});


	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,
			                    Time::FromTimeT(3),
			                    Time::FromTimeT(4));
		});


	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TRUE(TimeEqual(start,Time::SinceEver()));
	EXPECT_TRUE(TimeEqual(end,Time::FromTimeT(1)));
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});

	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TRUE(TimeEqual(start,Time::FromTimeT(2)));
	EXPECT_TRUE(TimeEqual(end,Time::FromTimeT(3)));
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});
	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TRUE(TimeEqual(start,Time::FromTimeT(4)));
	EXPECT_TRUE(TimeEqual(end,Time::Forever()));
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});
	EXPECT_FALSE(zone->NextFreeTimeRegion(start,end));



}




} // namespace priv
} // namespace myrmidon
} // namespace fort
