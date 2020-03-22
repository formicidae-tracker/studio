#include "AntMetadataUTest.hpp"

#include <gmock/gmock.h>

#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>



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

	EXPECT_EQ(metadata->Count("foo"), 1);
	EXPECT_EQ(metadata->Count("bar"), 1);
	EXPECT_EQ(metadata->Count("baz"), 1);
	EXPECT_EQ(metadata->Count("foobar"), 0);


	EXPECT_THROW({
			metadata->Delete("foobar");
		},std::out_of_range);

	EXPECT_NO_THROW({
			metadata->Delete("foo");
		});

	EXPECT_EQ(metadata->Count("foo"),0);
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

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type(42),true),std::invalid_argument);
}


TEST_F(AntMetadataUTest,DataTypeStringConversion) {

	EXPECT_NO_THROW({
			EXPECT_TRUE(std::get<bool>(AntMetadata::FromString(AntMetadata::Type::Bool,"true")));
		});
	EXPECT_NO_THROW({
			EXPECT_FALSE(std::get<bool>(AntMetadata::FromString(AntMetadata::Type::Bool,"false")));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::Bool,"");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<int>(AntMetadata::FromString(AntMetadata::Type::Int,"-12345")),-12345);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::Int,"foo");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_DOUBLE_EQ(std::get<double>(AntMetadata::FromString(AntMetadata::Type::Double,"0.69e-6")),0.69e-6);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::Double,"foo");},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<std::string>(AntMetadata::FromString(AntMetadata::Type::String,"foobar")),"foobar");
		});

	EXPECT_NO_THROW({
			auto dateStr = "2019-11-02T23:46:23.000Z";
			EXPECT_TRUE(TimeEqual(std::get<Time>(AntMetadata::FromString(AntMetadata::Type::Time,dateStr)),Time::Parse(dateStr)));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::Double,"foo");},std::invalid_argument);

	EXPECT_THROW(AntMetadata::FromString(AntMetadata::Type(42),"foo"),std::invalid_argument);

}


TEST_F(AntMetadataUTest,DataTypeStringValidation) {
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Bool,"true"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Bool,"false"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Bool,"true2"),AntMetadata::Validity::Invalid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Bool,"tru"),AntMetadata::Validity::Intermediate);


	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"-123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"+"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Int,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"1.2345e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"-1.234e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"1.234e-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Double,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::String,"sdbi wi \n fo"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Time,"2019-11-02T23:56:02.123456Z"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::Time,"<any-string>"),AntMetadata::Validity::Intermediate);

	EXPECT_THROW(AntMetadata::Validate(AntMetadata::Type(42), ""),std::invalid_argument);
}


TEST_F(AntMetadataUTest,ColumnPropertyCallbacks) {
	class MockAntMetadataCallback {
	public:
		MOCK_METHOD(void,OnNameChange,(const std::string &, const std::string),());
		MOCK_METHOD(void,OnTypeChange,(const std::string &, AntMetadata::Type, AntMetadata::Type),());
	};

	MockAntMetadataCallback callbacks;
	metadata = std::make_shared<AntMetadata>([&callbacks](const std::string & oldName,
	                                                     const std::string & newName) {
		                                         callbacks.OnNameChange(oldName,newName);
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                     AntMetadata::Type oldType,
	                                                     AntMetadata::Type newType) {
		                                         callbacks.OnTypeChange(name,oldType,newType);
	                                         });


	auto column = AntMetadata::Create(metadata,
	                                  "foo",
	                                  AntMetadata::Type::Bool);
	EXPECT_CALL(callbacks,OnNameChange("foo","bar")).Times(1);
	EXPECT_CALL(callbacks,OnTypeChange("bar",AntMetadata::Type::Bool,AntMetadata::Type::Int)).Times(1);
	column->SetName("bar");
	ASSERT_EQ(column->Name(),"bar");
	column->SetMetadataType(AntMetadata::Type::Int);
	ASSERT_EQ(column->MetadataType(),AntMetadata::Type::Int);

	auto toDel = AntMetadata::Create(metadata,"foo",AntMetadata::Type::Bool);
	metadata->Delete("foo");


	auto & columns = const_cast<AntMetadata::ColumnByName&>(metadata->Columns());
	columns.insert(std::make_pair("baz",column));
	columns.erase("bar");
	EXPECT_THROW(column->SetName("foobar"),std::logic_error);

	metadata.reset();

	EXPECT_THROW(column->SetName("baz"),DeletedReference<AntMetadata>);
	EXPECT_THROW(column->SetMetadataType(AntMetadata::Type::Bool),DeletedReference<AntMetadata>);
	column.reset();

}



} // namespace priv
} // namespace myrmidon
} // namespace fort
