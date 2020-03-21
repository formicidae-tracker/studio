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

TEST_F(AntMetadataUTest,DataTypeChecking) {
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::Bool,true));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Bool,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Bool,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Bool,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Bool,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Int,true),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::Int,0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Int,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Int,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Int,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Double,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Double,0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::Double,0.0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Double,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Double,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::String,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::String,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::String,0.0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::String,std::string("foo")));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::String,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Time,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Time,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Time,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::Time,std::string("foo")),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::Time,Time::FromTimeT(0)));

}





} // namespace priv
} // namespace myrmidon
} // namespace fort
