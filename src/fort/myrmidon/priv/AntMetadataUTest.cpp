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
	EXPECT_NO_THROW(foo = AntMetadata::Create(metadata,"foo",AntMetadata::Type::BOOL););
	EXPECT_NO_THROW(bar = AntMetadata::Create(metadata,"bar",AntMetadata::Type::INT););
	EXPECT_NO_THROW(baz = AntMetadata::Create(metadata,"baz",AntMetadata::Type::STRING););

	EXPECT_THROW({
	              foo->SetName("bar");
		},std::invalid_argument);

	EXPECT_THROW({
			AntMetadata::Create(metadata,"foo",AntMetadata::Type::BOOL);
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadata::Create(metadata,"foo",AntMetadata::Type::STRING);
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadata::Column::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::Create(metadata,"foo",AntMetadata::Type::BOOL););
	EXPECT_NO_THROW(bar = AntMetadata::Create(metadata,"bar",AntMetadata::Type::INT););
	EXPECT_NO_THROW(baz = AntMetadata::Create(metadata,"baz",AntMetadata::Type::STRING););

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
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::BOOL,true));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::BOOL,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::BOOL,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::BOOL,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::BOOL,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::INT,true),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::INT,0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::INT,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::INT,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::INT,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::DOUBLE,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::DOUBLE,0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::DOUBLE,0.0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::DOUBLE,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::DOUBLE,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::STRING,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::STRING,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::STRING,0.0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::STRING,std::string("foo")));
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::STRING,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::TIME,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::TIME,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::TIME,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type::TIME,std::string("foo")),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetadata::Type::TIME,Time::FromTimeT(0)));

	EXPECT_THROW(AntMetadata::CheckType(AntMetadata::Type(42),true),std::invalid_argument);
}


TEST_F(AntMetadataUTest,DataTypeStringConversion) {

	EXPECT_NO_THROW({
			EXPECT_TRUE(std::get<bool>(AntMetadata::FromString(AntMetadata::Type::BOOL,"true")));
		});
	EXPECT_NO_THROW({
			EXPECT_FALSE(std::get<bool>(AntMetadata::FromString(AntMetadata::Type::BOOL,"false")));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::BOOL,"");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<int>(AntMetadata::FromString(AntMetadata::Type::INT,"-12345")),-12345);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::INT,"foo");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_DOUBLE_EQ(std::get<double>(AntMetadata::FromString(AntMetadata::Type::DOUBLE,"0.69e-6")),0.69e-6);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<std::string>(AntMetadata::FromString(AntMetadata::Type::STRING,"foobar")),"foobar");
		});

	EXPECT_NO_THROW({
			auto dateStr = "2019-11-02T23:46:23.000Z";
			EXPECT_TRUE(TimeEqual(std::get<Time>(AntMetadata::FromString(AntMetadata::Type::TIME,dateStr)),Time::Parse(dateStr)));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetadata::Type::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_THROW(AntMetadata::FromString(AntMetadata::Type(42),"foo"),std::invalid_argument);

}


TEST_F(AntMetadataUTest,DataTypeStringValidation) {
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::BOOL,"true"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::BOOL,"false"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::BOOL,"true2"),AntMetadata::Validity::Invalid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::BOOL,"tru"),AntMetadata::Validity::Intermediate);


	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"-123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"+"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::INT,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"1.2345e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"-1.234e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"1.234e-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::DOUBLE,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::STRING,"sdbi wi \n fo"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::TIME,"2019-11-02T23:56:02.123456Z"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetadata::Type::TIME,"<any-string>"),AntMetadata::Validity::Intermediate);

	EXPECT_THROW(AntMetadata::Validate(AntMetadata::Type(42), ""),std::invalid_argument);
}


TEST_F(AntMetadataUTest,ColumnPropertyCallbacks) {
	class MockAntMetadataCallback {
	public:
		MOCK_METHOD(void,OnNameChange,(const std::string &, const std::string),());
		MOCK_METHOD(void,OnTypeChange,(const std::string &, AntMetadata::Type, AntMetadata::Type),());
		MOCK_METHOD(void,OnDefaultChange,(const std::string &, const AntStaticValue &, const AntStaticValue&),());
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
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                      const AntStaticValue & oldDefault,
	                                                      const AntStaticValue & newDefault) {
		                                         callbacks.OnDefaultChange(name,oldDefault,newDefault);
	                                         });


	auto column = AntMetadata::Create(metadata,
	                                  "foo",
	                                  AntMetadata::Type::BOOL);
	EXPECT_CALL(callbacks,OnDefaultChange("foo",AntStaticValue(false),AntStaticValue(true))).Times(1);
	column->SetDefaultValue(true);


	EXPECT_CALL(callbacks,OnNameChange("foo","bar")).Times(1);
	EXPECT_CALL(callbacks,OnTypeChange("bar",AntMetadata::Type::BOOL,AntMetadata::Type::INT)).Times(1);
	EXPECT_CALL(callbacks,OnDefaultChange("bar",AntStaticValue(true),AntStaticValue(0))).Times(1);
	column->SetName("bar");
	ASSERT_EQ(column->Name(),"bar");
	column->SetMetadataType(AntMetadata::Type::INT);
	ASSERT_EQ(column->MetadataType(),AntMetadata::Type::INT);


	auto toDel = AntMetadata::Create(metadata,"foo",AntMetadata::Type::BOOL);
	metadata->Delete("foo");


	auto & columns = const_cast<AntMetadata::ColumnByName&>(metadata->Columns());
	columns.insert(std::make_pair("baz",column));
	columns.erase("bar");
	EXPECT_THROW(column->SetName("foobar"),std::logic_error);

	metadata.reset();

	EXPECT_THROW(column->SetName("baz"),DeletedReference<AntMetadata>);
	EXPECT_THROW(column->SetMetadataType(AntMetadata::Type::BOOL),DeletedReference<AntMetadata>);
	column.reset();

}

TEST_F(AntMetadataUTest,ColumnHaveDefaults) {
	AntMetadata::Column::Ptr boolCol,intCol,doubleCol,stringCol,timeCol;
	ASSERT_NO_THROW({
			boolCol = AntMetadata::Create(metadata,"bool",AntMetadata::Type::BOOL);
			intCol = AntMetadata::Create(metadata,"int",AntMetadata::Type::INT);
			doubleCol = AntMetadata::Create(metadata,"double",AntMetadata::Type::DOUBLE);
			stringCol = AntMetadata::Create(metadata,"string",AntMetadata::Type::STRING);
			timeCol = AntMetadata::Create(metadata,"timeCol",AntMetadata::Type::TIME);
		});

	EXPECT_EQ(boolCol->DefaultValue(),AntStaticValue(false));
	EXPECT_EQ(intCol->DefaultValue(),AntStaticValue(0));
	EXPECT_EQ(doubleCol->DefaultValue(),AntStaticValue(0.0));
	EXPECT_EQ(stringCol->DefaultValue(),AntStaticValue(std::string()));
	EXPECT_EQ(timeCol->DefaultValue(),AntStaticValue(Time()));

	EXPECT_THROW({
			boolCol->SetDefaultValue(0.0);
		},std::bad_variant_access);
	EXPECT_NO_THROW({
			boolCol->SetDefaultValue(true);
		});
	EXPECT_NO_THROW({
			boolCol->SetMetadataType(AntMetadata::Type::TIME);
		});

	EXPECT_EQ(boolCol->DefaultValue(),AntStaticValue(Time()));

}



} // namespace priv
} // namespace myrmidon
} // namespace fort
