#include "MatchersUTest.hpp"

#include "Matchers.hpp"
#include "Experiment.hpp"
#include "Ant.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <gmock/gmock.h>

using ::testing::_;

namespace fort {
namespace myrmidon {
namespace priv {

class StaticMatcher : public Matcher {
private:
	bool d_value;
public:
	StaticMatcher(bool value) : d_value(value) {};

	void SetUpOnce(const ConstAntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const CollisionFrame::ConstPtr & collisionFrame) override{
	};

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const std::vector<fort::myrmidon::InteractionType> & types) override {
		return d_value;
	};

	void Format(std::ostream & out) const override {
		out << std::boolalpha << d_value;
	}

	static Ptr Create(bool value) {
		return std::make_shared<StaticMatcher>(value);
	}

};

class MockMatcher : public Matcher {
public:
	MOCK_METHOD(void,SetUpOnce,(const ConstAntByID & ants),(override));
	MOCK_METHOD(void,SetUp,(const IdentifiedFrame::ConstPtr & f,
	                        const CollisionFrame::ConstPtr & i),(override));
	MOCK_METHOD(bool,Match,(fort::myrmidon::AntID a,
	                        fort::myrmidon::AntID b,
	                        const std::vector<fort::myrmidon::InteractionType> & types), (override));
	MOCK_METHOD(void,Format,(std::ostream & out), (const override));
};

TEST_F(MatchersUTest,AndMatcher) {
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(false),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(false),StaticMatcher::Create(true)})->Match(0,0,{}));
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(true),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::And({StaticMatcher::Create(true),StaticMatcher::Create(true)})->Match(0,0,{}));
}

TEST_F(MatchersUTest,OrMatcher) {
	EXPECT_FALSE(Matcher::Or({StaticMatcher::Create(false),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(false),StaticMatcher::Create(true)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(true),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(true),StaticMatcher::Create(true)})->Match(0,0,{}));
}


TEST_F(MatchersUTest,CombinedMatchersSetup) {
	std::vector<std::shared_ptr<MockMatcher>> matchers
		= {
		   std::make_shared<MockMatcher>(),
		   std::make_shared<MockMatcher>(),
	};

	EXPECT_CALL(*matchers[0],SetUpOnce(_)).Times(2);
	EXPECT_CALL(*matchers[1],SetUpOnce(_)).Times(2);

	EXPECT_CALL(*matchers[0],SetUp(_,_)).Times(2);
	EXPECT_CALL(*matchers[1],SetUp(_,_)).Times(2);


	auto andMatcher = Matcher::And({matchers[0],matchers[1]});
	auto orMatcher = Matcher::Or({matchers[0],matchers[1]});

	andMatcher->SetUpOnce({});
	orMatcher->SetUpOnce({});

	andMatcher->SetUp({},{});
	orMatcher->SetUp({},{});

	andMatcher.reset();
	orMatcher.reset();
	matchers.clear();

}


TEST_F(MatchersUTest,IDMatcher) {
	auto idMatcher = Matcher::AntIDMatcher(42);
	ASSERT_NO_THROW({
			idMatcher->SetUpOnce({});
			idMatcher->SetUp({},{});
		});

	EXPECT_TRUE(idMatcher->Match(42,0,{}));
	EXPECT_FALSE(idMatcher->Match(0,0,{}));
}

TEST_F(MatchersUTest,ColumnMatcher) {
	auto columnMatcher = Matcher::AntColumnMatcher("bar",42);

	auto experiment = Experiment::Create(TestSetup::Basedir() / "matcher.myrmidon");
	experiment->AddAntMetadataColumn("bar",AntMetadata::Type::INT);

	auto a = experiment->CreateAnt();
	a->SetValue("bar",42,Time::ConstPtr());
	a->SetValue("bar",43,std::make_shared<Time>());

	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->FrameTime = Time().Add(-1);
	auto collisionFrame = std::make_shared<CollisionFrame>();
	ASSERT_NO_THROW({
			columnMatcher->SetUpOnce(experiment->CIdentifier().CAnts());
		});

	ASSERT_THROW({
			columnMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_NO_THROW({
			columnMatcher->SetUp(identifiedFrame,{});
		});

	EXPECT_FALSE(columnMatcher->Match(0,0,{}));
	EXPECT_TRUE(columnMatcher->Match(a->AntID(),0,{}));
	ASSERT_NO_THROW({
			columnMatcher->SetUp({},collisionFrame);
		});

	EXPECT_FALSE(columnMatcher->Match(a->AntID(),0,{}));

}


TEST_F(MatchersUTest,DistanceMatcher) {
	auto greaterMatcher = Matcher::AntDistanceGreaterThan(10);
	auto smallerMatcher = Matcher::AntDistanceSmallerThan(10);


	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->Positions.push_back({{0,0},0,1});
	identifiedFrame->Positions.push_back({{0,12},0,2});
	identifiedFrame->Positions.push_back({{0,8},0,3});
	auto collisionFrame = std::make_shared<CollisionFrame>();

	ASSERT_NO_THROW({ greaterMatcher->SetUpOnce({}); });

	ASSERT_THROW({
			greaterMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_THROW({
			smallerMatcher->SetUp({},collisionFrame);
		},std::runtime_error);

	ASSERT_NO_THROW({
			greaterMatcher->SetUp(identifiedFrame,{});
			smallerMatcher->SetUp(identifiedFrame,{});
		});


	EXPECT_TRUE(greaterMatcher->Match(0,0,{}));
	EXPECT_TRUE(greaterMatcher->Match(1,2,{}));
	EXPECT_FALSE(greaterMatcher->Match(1,3,{}));

	EXPECT_TRUE(smallerMatcher->Match(0,0,{}));
	EXPECT_FALSE(smallerMatcher->Match(1,2,{}));
	EXPECT_TRUE(smallerMatcher->Match(1,3,{}));


}


TEST_F(MatchersUTest,AngleMatcher) {
	auto greaterMatcher = Matcher::AntAngleGreaterThan(M_PI/4);
	auto smallerMatcher = Matcher::AntAngleSmallerThan(M_PI/4);


	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->Positions.push_back({{0,0},0,1});
	identifiedFrame->Positions.push_back({{0,0},M_PI/5,2});
	identifiedFrame->Positions.push_back({{0,0},-M_PI/3,3});
	auto collisionFrame = std::make_shared<CollisionFrame>();

	ASSERT_NO_THROW({ greaterMatcher->SetUpOnce({}); });

	ASSERT_THROW({
			greaterMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_THROW({
			smallerMatcher->SetUp({},collisionFrame);
		},std::runtime_error);

	ASSERT_NO_THROW({
			greaterMatcher->SetUp(identifiedFrame,{});
			smallerMatcher->SetUp(identifiedFrame,{});
		});


	EXPECT_TRUE(greaterMatcher->Match(0,0,{}));
	EXPECT_FALSE(greaterMatcher->Match(1,2,{}));
	EXPECT_TRUE(greaterMatcher->Match(1,3,{}));

	EXPECT_TRUE(smallerMatcher->Match(0,0,{}));
	EXPECT_TRUE(smallerMatcher->Match(1,2,{}));
	EXPECT_FALSE(smallerMatcher->Match(1,3,{}));


}


TEST_F(MatchersUTest,Formatting) {
	struct TestData {
		Matcher::Ptr M;
		std::string Expected;
	};

	std::vector<TestData> testdata
		= {
		   {
		    Matcher::AntIDMatcher(1),
		    "Ant.ID == 0x0001",
		   },
		   {
		    Matcher::AntColumnMatcher("foo",42.3),
		    "Ant.'foo' == 42.3",
		   },
		   {
		    Matcher::AntDistanceSmallerThan(42.3),
		    "Distance(Ant1, Ant2) < 42.3",
		   },
		   {
		    Matcher::AntDistanceGreaterThan(42.3),
		    "Distance(Ant1, Ant2) > 42.3",
		   },
		   {
		    Matcher::AntAngleSmallerThan(0.1),
		    "Angle(Ant1, Ant2) < 0.1",
		   },
		   {
		    Matcher::AntAngleGreaterThan(0.1),
		    "Angle(Ant1, Ant2) > 0.1",
		   },
		   {
		    Matcher::And({StaticMatcher::Create(false),
		                  StaticMatcher::Create(true),
		                  StaticMatcher::Create(false)}),
		    "( false && true && false )",
		   },
		   {
		    Matcher::Or({StaticMatcher::Create(false),
		                  StaticMatcher::Create(true),
		                  StaticMatcher::Create(false)}),
		    "( false || true || false )",
		   },

	};

	for ( const auto & d : testdata ) {
		std::ostringstream oss;
		oss << *d.M;
		EXPECT_EQ(oss.str(),d.Expected);
	}
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
