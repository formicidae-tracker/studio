#include "AntMetadata.hpp"

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>
#include <fort/myrmidon/utils/StringManipulation.hpp>

#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace fort {
namespace myrmidon {
namespace priv {

void AntMetadata::CheckName(const std::string & name) const {
	auto fi = d_columns.find(name);
	if ( fi != d_columns.cend() ) {
		throw std::invalid_argument("Column name '" + name + "' is already used");
	}
}

AntMetadata::Column::Ptr AntMetadata::Create(const Ptr & itself,
                                             const std::string & name,
                                             AntMetadata::Type type) {
	itself->CheckName(name);

	auto res = std::make_shared<AntMetadata::Column>(itself,name,type);
	itself->d_columns.insert(std::make_pair(name,res));
	return res;
}

void AntMetadata::Delete(const std::string & name) {
	auto fi =  d_columns.find(name);
	if ( fi == d_columns.end() ) {
		throw std::out_of_range("Unmanaged column '" + name + "'");
	}
	d_columns.erase(fi);
}

AntMetadata::AntMetadata()
	: d_onNameChange([](const std::string &, const std::string) {} )
	, d_onTypeChange([](const std::string &, Type, Type) {} ) {
}

AntMetadata::AntMetadata(const NameChangeCallback & onNameChange,
                         const TypeChangeCallback & onTypeChange)
	: d_onNameChange(onNameChange)
	, d_onTypeChange(onTypeChange) {
}

const AntMetadata::ColumnByName & AntMetadata::Columns() const {
	return d_columns;
}

size_t AntMetadata::Count(const std::string & name) const {
	return d_columns.count(name);
}


AntMetadata::Validity AntMetadata::Validate(Type type, const std::string & value) {
	std::vector<std::function< Validity (const std::string & value) > > validators =
		{
		 [](const std::string & value) {
			 if ( value == "true" || value == "false" ) {
				 return Validity::Valid;
			 }
			 if ( utils::HasPrefix("true",value) == true
			      || utils::HasPrefix("false",value) == true ) {
				 return Validity::Intermediate;
			 }
			 return Validity::Invalid;
		 },
		 [](const std::string & value) {
			 try {
				 std::stoi(value);
				 return Validity::Valid;
			 } catch ( const std::exception & e) {
				 if ( value == "+" || value == "-" ) {
					 return Validity::Intermediate;
				 }
			 }
			 return Validity::Invalid;
		 },
		 [](const std::string & value) {
			 try {
				 size_t pos;
				 std::stod(value,&pos);
				 if ( pos == value.size() ) {
					 return Validity::Valid;
				 }
				 return Validity::Intermediate;
			 } catch ( const std::exception & e) {
				 if ( value == "+" || value == "-" ) {
					 return Validity::Intermediate;
				 }
			 }
			 return Validity::Invalid;
		 },
		 [](const std::string & value) {
			 return Validity::Valid;
		 },
		 [](const std::string & value) {
			 try {
				 Time::Parse(value);
				 return Validity::Valid;
			 } catch ( const std::exception & e ) {
				 return Validity::Intermediate;
			 }
		 },
		};
	size_t idx = size_t(type);
	if ( idx >= validators.size() ) {
		throw std::invalid_argument("Unknown AntMetadata::Type value " + std::to_string(idx));
	}
	return validators[idx](value);
}

void AntMetadata::CheckType(Type type, const AntStaticValue & data) {
	static std::vector<std::function<void (const AntStaticValue &)>> checkers =
		{
		 [](const AntStaticValue & data) { std::get<bool>(data); },
		 [](const AntStaticValue & data) { std::get<int32_t>(data); },
		 [](const AntStaticValue & data) { std::get<double>(data); },
		 [](const AntStaticValue & data) { std::get<std::string>(data); },
		 [](const AntStaticValue & data) { std::get<Time>(data); },
		};
	size_t idx = size_t(type);
	if ( idx >= checkers.size() ) {
		throw std::invalid_argument("Unknown AntMetadata::Type value " + std::to_string(idx));
	}
	checkers[idx](data);
}

AntStaticValue AntMetadata::FromString(Type type, const std::string & value) {
	static std::vector<std::function<AntStaticValue (const std::string &)>> converters =
		{
		 [](const std::string & value ) ->  AntStaticValue {
			 std::istringstream iss(value);
			 bool res;
			 iss >> std::boolalpha >> res;
			 if ( iss.good() == false ) {
				 throw std::invalid_argument("Invalid string '" + value + "' for AntMetadata::Value");
			 }
			 return res;
		 },
		 [](const std::string & value ) ->  AntStaticValue {
			 return std::stoi(value);
		 },
		 [](const std::string & value ) ->  AntStaticValue {
			 return std::stod(value);
		 },
		 [](const std::string & value ) ->  AntStaticValue {
			 return value;
		 },
		 [](const std::string & value ) ->  AntStaticValue {
			 return Time::Parse(value);
		 },
		};
	size_t idx = size_t(type);
	if ( idx >= converters.size() ) {
		throw std::invalid_argument("Unknown AntMetadata::Type value " + std::to_string(idx));
	}
	return converters[idx](value);
}

AntStaticValue AntMetadata::DefaultValue(Type type) {
	static std::vector<AntStaticValue> defaults =
		{
		 false,
		 0,
		 0.0,
		 std::string(""),
		 Time(),
		};
	size_t idx = size_t(type);
	if ( idx >= defaults.size() ) {
		throw std::invalid_argument("Unknown AntMetadata::Type value " + std::to_string(idx));
	}
	return defaults[idx];

}


const std::string & AntMetadata::Column::Name() const {
	return d_name;
}

void AntMetadata::Column::SetName(const std::string & name) {
	auto metadata = d_metadata.lock();
	if ( !metadata ) {
		throw DeletedReference<AntMetadata>();
	}
	metadata->CheckName(name);
	auto fi = metadata->d_columns.find(d_name);
	if ( fi == metadata->d_columns.end() ) {
		throw std::logic_error("column '" + d_name + "' is not managed by its manager");
	}
	metadata->d_columns.insert(std::make_pair(name,fi->second));
	metadata->d_columns.erase(d_name);
	metadata->d_onNameChange(d_name,name);
	d_name = name;
}

AntMetadata::Type AntMetadata::Column::MetadataType() const {
	return d_type;
}

void AntMetadata::Column::SetMetadataType(AntMetadata::Type type){
	auto metadata = d_metadata.lock();
	if ( !metadata ) {
		throw DeletedReference<AntMetadata>();
	}
	metadata->d_onTypeChange(d_name,d_type,type);

	d_type = type;
}

AntMetadata::Column::Column(const std::weak_ptr<AntMetadata> & metadata,
                            const std::string & name,
                            AntMetadata::Type type)
	: d_metadata(metadata)
	, d_name(name)
	, d_type(type) {
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
