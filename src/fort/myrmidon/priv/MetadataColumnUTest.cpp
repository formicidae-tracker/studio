#include "MetadataColumnUTest.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void MetadataColumnUTest::SetUp() {
	manager =  std::make_shared<MetadataColumn::Manager>();
}
void MetadataColumnUTest::TearDown() {
	manager.reset();
}


TEST_F(MetadataColumnUTest,ColumnHaveUniqueName) {
	MetadataColumn::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = MetadataColumn::Manager::Create(manager,"foo",MetadataColumn::Type::Bool););
	EXPECT_NO_THROW(bar = MetadataColumn::Manager::Create(manager,"bar",MetadataColumn::Type::Int););
	EXPECT_NO_THROW(baz = MetadataColumn::Manager::Create(manager,"baz",MetadataColumn::Type::String););

	EXPECT_THROW({
	              foo->SetName("bar");
		},std::invalid_argument);

	EXPECT_THROW({
			MetadataColumn::Manager::Create(manager,"foo",MetadataColumn::Type::Bool);
		},std::invalid_argument);

	foo->SetName("foobar");
	MetadataColumn::Manager::Create(manager,"foo",MetadataColumn::Type::String);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
