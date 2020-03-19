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

TEST_F(MetadataColumnUTest,ColumnAdditionDeletion) {
	MetadataColumn::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = MetadataColumn::Manager::Create(manager,"foo",MetadataColumn::Type::Bool););
	EXPECT_NO_THROW(bar = MetadataColumn::Manager::Create(manager,"bar",MetadataColumn::Type::Int););
	EXPECT_NO_THROW(baz = MetadataColumn::Manager::Create(manager,"baz",MetadataColumn::Type::String););

	EXPECT_THROW({
			manager->Delete("foobar");
		},std::out_of_range);

	EXPECT_NO_THROW({
			manager->Delete("foo");
		});

	EXPECT_THROW({
			manager->Delete("foo");
		},std::out_of_range);

}

TEST_F(MetadataColumnUTest,DataTypeConversion) {
	EXPECT_FALSE(MetadataColumn::ToBool("FALSE"));
	EXPECT_FALSE(MetadataColumn::ToBool("FaLSe"));
	EXPECT_FALSE(MetadataColumn::ToBool(""));
	EXPECT_TRUE(MetadataColumn::ToBool("FALS"));
	EXPECT_TRUE(MetadataColumn::ToBool("TRUE"));

	EXPECT_EQ(MetadataColumn::ToInt(""),0);
	EXPECT_EQ(MetadataColumn::ToInt("dvwhidbqoedbqw  qiqw dlqwo "),0);
	EXPECT_EQ(MetadataColumn::ToInt("123456"),123456);
	EXPECT_EQ(MetadataColumn::ToInt("-123456"),-123456);
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
