#include "IdentifierUTest.hpp"

#include "Identifier.hpp"
#include "Ant.hpp"

using namespace fort::myrmidon::priv;

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
}
