#include "AntMetadataUTest.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void AntMetadataUTest::SetUp() {
	list =  std::make_shared<AntMetadata::UniqueColumnList>();
}
void AntMetadataUTest::TearDown() {
	list.reset();
}


TEST_F(AntMetadataUTest,ColumnHaveUniqueName) {
	AntMetadata::Column::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::UniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadata::UniqueColumnList::Create(list,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadata::UniqueColumnList::Create(list,"baz",AntMetadata::Type::String););

	EXPECT_THROW({
	              foo->SetName("bar");
		},std::invalid_argument);

	EXPECT_THROW({
			AntMetadata::UniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool);
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadata::UniqueColumnList::Create(list,"foo",AntMetadata::Type::String);
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadata::Column::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::UniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadata::UniqueColumnList::Create(list,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadata::UniqueColumnList::Create(list,"baz",AntMetadata::Type::String););

	EXPECT_EQ(list->Columns().size(),3);

	EXPECT_THROW({
			list->Delete("foobar");
		},std::out_of_range);

	EXPECT_NO_THROW({
			list->Delete("foo");
		});

	EXPECT_EQ(list->Columns().size(),2);

	EXPECT_THROW({
			list->Delete("foo");
		},std::out_of_range);

	EXPECT_EQ(list->Columns().size(),2);
}

TEST_F(AntMetadataUTest,DataTypeConversion) {
	EXPECT_FALSE(AntMetadata::Column::ToBool("FALSE"));
	EXPECT_FALSE(AntMetadata::Column::ToBool("FaLSe"));
	EXPECT_FALSE(AntMetadata::Column::ToBool(""));
	EXPECT_TRUE(AntMetadata::Column::ToBool("FALS"));
	EXPECT_TRUE(AntMetadata::Column::ToBool("TRUE"));

	EXPECT_EQ(AntMetadata::Column::ToInt(""),0);
	EXPECT_EQ(AntMetadata::Column::ToInt("dvwhidbqoedbqw  qiqw dlqwo "),0);
	EXPECT_EQ(AntMetadata::Column::ToInt("123456"),123456);
	EXPECT_EQ(AntMetadata::Column::ToInt("-123456"),-123456);

	EXPECT_EQ(AntMetadata::Column::FromValue(true),"TRUE");
	EXPECT_EQ(AntMetadata::Column::FromValue(false),"FALSE");

	EXPECT_EQ(AntMetadata::Column::FromValue(0),"0");
	EXPECT_EQ(AntMetadata::Column::FromValue(-12345),"-12345");
}





} // namespace priv
} // namespace myrmidon
} // namespace fort
