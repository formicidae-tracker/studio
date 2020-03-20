#include "AntMetadataUTest.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void AntMetadataUTest::SetUp() {
	metadata =  std::make_shared<AntMetadata>();
}
void AntMetadataUTest::TearDown() {
	metadata.reset();
}


TEST_F(AntMetadataUTest,ColumnHaveUniqueName) {
	AntMetadata::Column::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::Create(metadata,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadata::Create(metadata,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadata::Create(metadata,"baz",AntMetadata::Type::String););

	EXPECT_THROW({
	              foo->SetName("bar");
		},std::invalid_argument);

	EXPECT_THROW({
			AntMetadata::Create(metadata,"foo",AntMetadata::Type::Bool);
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadata::Create(metadata,"foo",AntMetadata::Type::String);
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadata::Column::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::Create(metadata,"foo",AntMetadata::Type::Bool););
	EXPECT_NO_THROW(bar = AntMetadata::Create(metadata,"bar",AntMetadata::Type::Int););
	EXPECT_NO_THROW(baz = AntMetadata::Create(metadata,"baz",AntMetadata::Type::String););

	EXPECT_EQ(metadata->Columns().size(),3);

	EXPECT_THROW({
			metadata->Delete("foobar");
		},std::out_of_range);

	EXPECT_NO_THROW({
			metadata->Delete("foo");
		});

	EXPECT_EQ(metadata->Columns().size(),2);

	EXPECT_THROW({
			metadata->Delete("foo");
		},std::out_of_range);

	EXPECT_EQ(metadata->Columns().size(),2);
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
