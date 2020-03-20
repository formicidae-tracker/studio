#include "AntMetadataUTest.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void AntMetadataUTest::SetUp() {
	list =  std::make_shared<AntMetadataUniqueColumnList>();
}
void AntMetadataUTest::TearDown() {
	list.reset();
}


TEST_F(AntMetadataUTest,ColumnHaveUniqueName) {
	AntMetadataColumn::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadataUniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadataUniqueColumnList::Create(list,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadataUniqueColumnList::Create(list,"baz",AntMetadata::Type::String););

	EXPECT_THROW({
	              foo->SetName("bar");
		},std::invalid_argument);

	EXPECT_THROW({
			AntMetadataUniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool);
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadataUniqueColumnList::Create(list,"foo",AntMetadata::Type::String);
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadataColumn::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadataUniqueColumnList::Create(list,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadataUniqueColumnList::Create(list,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadataUniqueColumnList::Create(list,"baz",AntMetadata::Type::String););

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
	EXPECT_FALSE(AntMetadata::ToBool("FALSE"));
	EXPECT_FALSE(AntMetadata::ToBool("FaLSe"));
	EXPECT_FALSE(AntMetadata::ToBool(""));
	EXPECT_TRUE(AntMetadata::ToBool("FALS"));
	EXPECT_TRUE(AntMetadata::ToBool("TRUE"));

	EXPECT_EQ(AntMetadata::ToInt(""),0);
	EXPECT_EQ(AntMetadata::ToInt("dvwhidbqoedbqw  qiqw dlqwo "),0);
	EXPECT_EQ(AntMetadata::ToInt("123456"),123456);
	EXPECT_EQ(AntMetadata::ToInt("-123456"),-123456);

	EXPECT_EQ(AntMetadata::FromValue(true),"TRUE");
	EXPECT_EQ(AntMetadata::FromValue(false),"FALSE");

	EXPECT_EQ(AntMetadata::FromValue(0),"0");
	EXPECT_EQ(AntMetadata::FromValue(-12345),"-12345");
}





} // namespace priv
} // namespace myrmidon
} // namespace fort
