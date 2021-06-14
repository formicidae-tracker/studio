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


TEST_F(AntMetadataUTest,KeyHaveUniqueName) {
	AntMetadata::Key::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::SetKey(metadata,"foo",false););
	EXPECT_NO_THROW(bar = AntMetadata::SetKey(metadata,"bar",12););
	EXPECT_NO_THROW(baz = AntMetadata::SetKey(metadata,"baz",std::string()););

	ASSERT_EQ(baz->Type(), AntMetaDataType::STRING);

	EXPECT_THROW({
			foo->SetName("bar");
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadata::SetKey(metadata,"foo",std::string(""));
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadata::Key::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::SetKey(metadata,"foo",AntMetadata::DefaultValue(AntMetaDataType::BOOL)););
	EXPECT_NO_THROW(bar = AntMetadata::SetKey(metadata,"bar",AntMetadata::DefaultValue(AntMetaDataType::INT)););
	EXPECT_NO_THROW(baz = AntMetadata::SetKey(metadata,"baz",AntMetadata::DefaultValue(AntMetaDataType::STRING)););

	EXPECT_EQ(metadata->Keys().size(),3);

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
	EXPECT_EQ(metadata->Keys().size(),2);

	EXPECT_THROW({
			metadata->Delete("foo");
		},std::out_of_range);

	EXPECT_EQ(metadata->Keys().size(),2);
}

TEST_F(AntMetadataUTest,DataTypeChecking) {
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetaDataType::BOOL,true));
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::BOOL,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::BOOL,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::BOOL,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::BOOL,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::INT,true),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetaDataType::INT,0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::INT,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::INT,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::INT,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::DOUBLE,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::DOUBLE,0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetaDataType::DOUBLE,0.0));
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::DOUBLE,std::string("foo")),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::DOUBLE,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::STRING,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::STRING,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::STRING,0.0),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetaDataType::STRING,std::string("foo")));
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::STRING,Time::FromTimeT(0)),std::bad_variant_access);

	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::TIME,true),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::TIME,0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::TIME,0.0),std::bad_variant_access);
	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType::TIME,std::string("foo")),std::bad_variant_access);
	EXPECT_NO_THROW(AntMetadata::CheckType(AntMetaDataType::TIME,Time::FromTimeT(0)));

	EXPECT_THROW(AntMetadata::CheckType(AntMetaDataType(42),true),std::invalid_argument);
}


TEST_F(AntMetadataUTest,DataTypeStringConversion) {

	EXPECT_NO_THROW({
			EXPECT_TRUE(std::get<bool>(AntMetadata::FromString(AntMetaDataType::BOOL,"true")));
		});
	EXPECT_NO_THROW({
			EXPECT_FALSE(std::get<bool>(AntMetadata::FromString(AntMetaDataType::BOOL,"false")));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetaDataType::BOOL,"");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<int>(AntMetadata::FromString(AntMetaDataType::INT,"-12345")),-12345);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetaDataType::INT,"foo");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_DOUBLE_EQ(std::get<double>(AntMetadata::FromString(AntMetaDataType::DOUBLE,"0.69e-6")),0.69e-6);
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetaDataType::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<std::string>(AntMetadata::FromString(AntMetaDataType::STRING,"foobar")),"foobar");
		});

	EXPECT_NO_THROW({
			auto dateStr = "2019-11-02T23:46:23.000Z";
			EXPECT_TRUE(TimeEqual(std::get<Time>(AntMetadata::FromString(AntMetaDataType::TIME,dateStr)),Time::Parse(dateStr)));
		});
	EXPECT_THROW({AntMetadata::FromString(AntMetaDataType::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_THROW(AntMetadata::FromString(AntMetaDataType(42),"foo"),std::invalid_argument);

}


TEST_F(AntMetadataUTest,DataTypeStringValidation) {
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::BOOL,"true"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::BOOL,"false"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::BOOL,"true2"),AntMetadata::Validity::Invalid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::BOOL,"tru"),AntMetadata::Validity::Intermediate);


	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"-123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"+"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::INT,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"1.2345e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"-1.234e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"1.234e-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::DOUBLE,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::STRING,"sdbi wi \n fo"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::TIME,"2019-11-02T23:56:02.123456Z"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(AntMetaDataType::TIME,"<any-string>"),AntMetadata::Validity::Intermediate);

	EXPECT_THROW(AntMetadata::Validate(AntMetaDataType(42), ""),std::invalid_argument);
}


TEST_F(AntMetadataUTest,ColumnPropertyCallbacks) {
	class MockAntMetadataCallback {
	public:
		MOCK_METHOD(void,OnNameChange,(const std::string &, const std::string),());
		MOCK_METHOD(void,OnTypeChange,(const std::string &, AntMetaDataType, AntMetaDataType),());
		MOCK_METHOD(void,OnDefaultChange,(const std::string &, const AntStaticValue &, const AntStaticValue&),());
	};

	MockAntMetadataCallback callbacks;
	metadata = std::make_shared<AntMetadata>([&callbacks](const std::string & oldName,
	                                                     const std::string & newName) {
		                                         callbacks.OnNameChange(oldName,newName);
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                     AntMetaDataType oldType,
	                                                     AntMetaDataType newType) {
		                                         callbacks.OnTypeChange(name,oldType,newType);
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                      const AntStaticValue & oldDefault,
	                                                      const AntStaticValue & newDefault) {
		                                         callbacks.OnDefaultChange(name,oldDefault,newDefault);
	                                         });


	auto column = AntMetadata::SetKey(metadata,
	                                  "foo",
	                                  false);
	EXPECT_CALL(callbacks,OnDefaultChange("foo",AntStaticValue(false),AntStaticValue(true))).Times(1);
	column->SetDefaultValue(true);


	EXPECT_CALL(callbacks,OnNameChange("foo","bar")).Times(1);
	EXPECT_CALL(callbacks,OnTypeChange("bar",AntMetaDataType::BOOL,AntMetaDataType::INT)).Times(1);
	EXPECT_CALL(callbacks,OnDefaultChange("bar",AntStaticValue(true),AntStaticValue(0))).Times(1);
	column->SetName("bar");
	ASSERT_EQ(column->Name(),"bar");
	column->SetDefaultValue(0);
	ASSERT_EQ(column->Type(),AntMetaDataType::INT);


	auto toDel = AntMetadata::SetKey(metadata,"foo",false);
	metadata->Delete("foo");


	auto & columns = const_cast<AntMetadata::KeysByName&>(metadata->Keys());
	columns.insert(std::make_pair("baz",column));
	columns.erase("bar");
	EXPECT_THROW(column->SetName("foobar"),std::logic_error);

	metadata.reset();

	EXPECT_THROW(column->SetName("baz"),DeletedReference<AntMetadata>);
	EXPECT_THROW(column->SetDefaultValue(false),DeletedReference<AntMetadata>);
	column.reset();

}

TEST_F(AntMetadataUTest,ColumnHaveDefaults) {
	AntMetadata::Key::Ptr boolCol,intCol,doubleCol,stringCol,timeCol;
	ASSERT_NO_THROW({
			boolCol = AntMetadata::SetKey(metadata,"bool",false);
			intCol = AntMetadata::SetKey(metadata,"int",0);
			doubleCol = AntMetadata::SetKey(metadata,"double",0.0);
			stringCol = AntMetadata::SetKey(metadata,"string",std::string());
			timeCol = AntMetadata::SetKey(metadata,"timeCol",fort::Time());
		});

	EXPECT_EQ(boolCol->DefaultValue(),AntStaticValue(false));
	EXPECT_EQ(intCol->DefaultValue(),AntStaticValue(0));
	EXPECT_EQ(doubleCol->DefaultValue(),AntStaticValue(0.0));
	EXPECT_EQ(stringCol->DefaultValue(),AntStaticValue(std::string()));
	EXPECT_EQ(timeCol->DefaultValue(),AntStaticValue(Time()));

	EXPECT_NO_THROW({
			boolCol->SetDefaultValue(true);
		});
	EXPECT_NO_THROW({
			boolCol->SetDefaultValue(fort::Time());
		});

	EXPECT_EQ(boolCol->DefaultValue(),AntStaticValue(Time()));

}



} // namespace priv
} // namespace myrmidon
} // namespace fort
