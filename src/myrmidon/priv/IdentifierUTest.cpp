#include "IdentifierUTest.hpp"

#include "Identifier.hpp"
#include "Ant.hpp"

#include <google/protobuf/util/time_util.h>

#include "../UtilsUTest.hpp"

#include <random>

namespace fort {
namespace myrmidon {
namespace priv {

typedef AlmostContiguousIDContainer<fort::myrmidon::Ant::ID,Ant::Ptr> Container;

TEST_F(IdentifierUTest,AntsAreCreatedSequentially) {
	auto i = Identifier::Create();

	try{
		size_t startSize = i->Ants().size();
		size_t toCreate = 5;

		for(size_t ii = 0; ii < toCreate; ++ii) {
			auto ant = i->CreateAnt();
			ASSERT_EQ(ant->ID(),i->Ants().size());
		}
		ASSERT_EQ(i->Ants().size(),startSize + toCreate);

		i->DeleteAnt(startSize+1);
		i->DeleteAnt(startSize+3);

		auto ant = i->CreateAnt();
		ASSERT_EQ(ant->ID(),startSize+1);

		ant = i->CreateAnt();
		ASSERT_EQ(ant->ID(),startSize+3);

		ant = i->CreateAnt();
		ASSERT_EQ(ant->ID(),i->Ants().size());

	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	EXPECT_THROW({
			i->CreateAnt(1);
		},Container::AlreadyExistingObject);
}

TEST_F(IdentifierUTest,MemoryRobust) {
	EXPECT_THROW({
			Identifier::Invalid().Itself();
		},DeletedReference<Identifier>);
}


TEST_F(IdentifierUTest,AntsCanBeDeleted) {
	auto i = Identifier::Create();
	AntPtr a;
	EXPECT_NO_THROW({
			a = i->CreateAnt();
		});

	EXPECT_THROW({
			i->DeleteAnt(a->ID()+1);
		}, Container::UnmanagedObject);

	IdentificationPtr ident;
	EXPECT_NO_THROW({
			ident = i->AddIdentification(a->ID(), 123, Time::ConstPtr(), Time::ConstPtr());
		});

	EXPECT_THROW({
			i->DeleteAnt(a->ID());
		}, std::logic_error);


	EXPECT_NO_THROW({
			i->DeleteIdentification(ident);
			i->DeleteAnt(a->ID());
		});
}


TEST_F(IdentifierUTest,AntCanBeAttachedToIdentification) {
	auto i = Identifier::Create();
	auto a = i->CreateAnt();
	EXPECT_THROW({
			i->AddIdentification(a->ID()+1,123,Time::ConstPtr(),Time::ConstPtr());
		},Container::UnmanagedObject);

	IdentificationPtr ident1,ident2;
	EXPECT_NO_THROW(ident1 = i->AddIdentification(a->ID(),123,Time::ConstPtr(),Time::ConstPtr()));

	auto ii = Identifier::Create();
	auto aa = ii->CreateAnt();
	ident2 = ii->AddIdentification(aa->ID(),124,Time::ConstPtr(),Time::ConstPtr());

	EXPECT_THROW({
			i->DeleteIdentification(ident2);
		},Identifier::UnmanagedIdentification);

	EXPECT_NO_THROW(ii->DeleteIdentification(ident2));
	EXPECT_THROW(ii->DeleteIdentification(ident2),Identifier::UnmanagedIdentification);

}

TEST_F(IdentifierUTest,CanIdentifyAntByTag) {
	auto i = Identifier::Create();
	auto a = i->CreateAnt();
	auto start = std::make_shared<const Time>(Time::Parse("2019-11-02T22:00:20.021+01:00"));
	auto end = std::make_shared<const Time>(Time::Parse("2019-11-02T22:30:25.863+01:00"));
	auto secondStart = std::make_shared<const Time>(Time::Parse("2019-11-02T22:34:25.412+01:00"));
	auto ident = i->AddIdentification(a->ID(),123,start,end);

	EXPECT_EQ(i->UseCount(123),1);
	EXPECT_EQ(i->UseCount(124),0);

	EXPECT_EQ(i->Identify(123,*start),ident);
	EXPECT_EQ(i->Identify(123,start->Add(1 * Duration::Minute)),ident);
	EXPECT_EQ(i->Identify(124,start->Add(1 * Duration::Minute)),IdentificationPtr());
	EXPECT_EQ(i->Identify(123,start->Add(-1 * Duration::Nanosecond)),IdentificationPtr());
	EXPECT_EQ(i->Identify(123,*end),IdentificationPtr());
	EXPECT_EQ(i->Identify(123,end->Add(-1)),ident);


	EXPECT_THROW({
			i->UpperUnidentifiedBound(123,*start);
		}, std::invalid_argument);
	EXPECT_EQ(i->UpperUnidentifiedBound(124,*start),Time::ConstPtr());
	EXPECT_EQ(i->UpperUnidentifiedBound(123,start->Add(-1)),start);
	EXPECT_EQ(i->UpperUnidentifiedBound(123,*end),Time::ConstPtr());

	EXPECT_THROW({
			i->LowerUnidentifiedBound(123,*start);
		}, std::invalid_argument);
	EXPECT_EQ(i->LowerUnidentifiedBound(124,*start),Time::ConstPtr());
	EXPECT_EQ(i->LowerUnidentifiedBound(123,*end),end);
	EXPECT_EQ(i->LowerUnidentifiedBound(123,start->Add(-1)),Time::ConstPtr());

	EXPECT_NO_THROW({
			i->AddIdentification(a->ID(),123,secondStart,Time::ConstPtr());
		});


	Time::ConstPtr freeStart,freeEnd;
	EXPECT_FALSE(i->FreeRangeContaining(freeStart,freeEnd,123,*start));
	EXPECT_TRUE(TimePtrEqual(freeStart,Time::ConstPtr()));
	EXPECT_TRUE(TimePtrEqual(freeEnd,Time::ConstPtr()));

	EXPECT_TRUE(i->FreeRangeContaining(freeStart,freeEnd,123,*end));
	EXPECT_TRUE(TimePtrEqual(freeStart,end));
	EXPECT_TRUE(TimePtrEqual(freeEnd,secondStart));

}


TEST_F(IdentifierUTest,Compilation) {
	  std::random_device r;
    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());

    std::uniform_int_distribution<uint32_t> duration(0, 600000);
    std::uniform_real_distribution<double> uniform(0, 1.0);
	auto identifier = Identifier::Create();
	std::set<Time,Time::Comparator> times;
	std::set<TagID> tags;
	const size_t NB_ANTS = 100;
	for ( size_t i = 0; i < NB_ANTS; ++i) {
		auto a = identifier->CreateAnt();
		std::set<Time,Time::Comparator> antTimes;

		while( uniform(e1) < 0.8 ) {
			antTimes.insert(Time::FromTimeT(0).Add(duration(e1) * Duration::Millisecond));
		}
		Time::ConstPtr lastTime;

		for ( const auto & t : antTimes ) {
			times.insert(t);
			auto tagID = NB_ANTS * a->Identifications().size() + i;
			tags.insert(tagID);
			auto end = std::make_shared<Time>(t);
			identifier->AddIdentification(a->ID(),tagID,
			                              lastTime,
			                              end);
			lastTime = end;
		}

		auto tagID = NB_ANTS * a->Identifications().size() + i;
		tags.insert(tagID);
		identifier->AddIdentification(a->ID(),tagID,
		                              lastTime,Time::ConstPtr());
	}

	auto start = Time::Now();
	auto compiled = identifier->Compile();
	auto end = Time::Now();

	auto testEqualityAtTime =
		[identifier,compiled,tags](const Time & time) -> ::testing::AssertionResult {
			for ( const auto & t : tags ) {
				auto start = Time::Now();
				auto expected = identifier->Identify(t,time);
				auto middle = Time::Now();
				auto res = compiled->Identify(t,time);
				auto end = Time::Now();

				if ( !expected ) {
					if ( !res == false ) {
						return ::testing::AssertionFailure() << " tag should not have been identified";
					}

					// if (middle.Sub(start).Nanoseconds() < end.Sub(middle).Nanoseconds()) {
					// 	return ::testing::AssertionFailure() <<
					// 		" Compiled time " << end.Sub(middle) <<
					// 		" is larger than flat time " << middle.Sub(start);
					// }

					return ::testing::AssertionSuccess();
				}
				if ( !res ) {
					return ::testing::AssertionFailure()
						<< "tag " << t << " should have been identified to "
						<< expected->Target()->ID() << " idents: "
						<< expected->Target()->Identifications().size();

				}

				if ( res->Target()->ID() != expected->Target()->ID() ) {
					return ::testing::AssertionFailure()
						<< "Got identification target mismatch, expected: "
						<< expected->Target()->ID()
						<< " got: " << res->Target()->ID();

				}
				// if (middle.Sub(start).Nanoseconds() < end.Sub(middle).Nanoseconds()) {
				// 	return ::testing::AssertionFailure() <<
				// 		" Compiled time " << end.Sub(middle) <<
				// 		" is larger than flat time " << middle.Sub(start);
				// }

			}
			return ::testing::AssertionSuccess();
		};

	size_t i  = 0;
	for ( const auto & t : times ) {
		//		EXPECT_TRUE(testEqualityAtTime(t.Add(-1))) << i;
		//EXPECT_TRUE(testEqualityAtTime(t)) << i;
		//EXPECT_TRUE(testEqualityAtTime(t.Add(1))) << i;
		++i;
	}

}


} // namespace fort
} // namespace myrmidon
} // namespace priv
