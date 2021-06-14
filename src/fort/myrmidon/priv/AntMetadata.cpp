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
	auto fi = d_keys.find(name);
	if ( fi != d_keys.cend() ) {
		throw std::invalid_argument("Key '" + name + "' is already used");
	}
}

AntMetadata::Key::Ptr AntMetadata::SetKey(const Ptr & itself,
                                          const std::string & name,
                                          AntStaticValue defaultValue) {
	auto fi = itself->d_keys.find(name);
	if ( fi != itself->d_keys.cend() ) {
		fi->second->SetDefaultValue(defaultValue);
		return fi->second;
	}

	auto res = std::make_shared<AntMetadata::Key>(itself,name,defaultValue);
	itself->d_keys.insert(std::make_pair(name,res));
	return res;
}

void AntMetadata::Delete(const std::string & name) {
	auto fi =  d_keys.find(name);
	if ( fi == d_keys.end() ) {
		throw std::out_of_range("Unknown key '" + name + "'");
	}
	d_keys.erase(fi);
}

AntMetadata::AntMetadata()
	: d_onNameChange([](const std::string &, const std::string) {} )
	, d_onTypeChange([](const std::string &, AntMetaDataType, AntMetaDataType) {})
	, d_onDefaultChange([](const std::string &, const AntStaticValue &, const AntStaticValue &) {} ) {
}

AntMetadata::AntMetadata(const NameChangeCallback & onNameChange,
                         const TypeChangeCallback & onTypeChange,
                         const DefaultChangeCallback & onDefaultChange )
	: d_onNameChange(onNameChange)
	, d_onTypeChange(onTypeChange)
	, d_onDefaultChange(onDefaultChange) {
}

const AntMetadata::KeysByName & AntMetadata::Keys() const {
	return d_keys;
}

size_t AntMetadata::Count(const std::string & name) const {
	return d_keys.count(name);
}



AntMetadata::Validity AntMetadata::Validate(AntMetaDataType type, const std::string & value) {
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

void AntMetadata::CheckType(AntMetaDataType type, const AntStaticValue & data) {
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

AntStaticValue AntMetadata::FromString(AntMetaDataType type, const std::string & value) {
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



const std::string & AntMetadata::Key::Name() const {
	return d_name;
}

void AntMetadata::Key::SetName(const std::string & name) {
	auto metadata = d_metadata.lock();
	if ( !metadata ) {
		throw DeletedReference<AntMetadata>();
	}
	metadata->CheckName(name);
	auto fi = metadata->d_keys.find(d_name);
	if ( fi == metadata->d_keys.end() ) {
		throw std::logic_error("Key '" + d_name + "' is not managed by its manager");
	}
	metadata->d_keys.insert(std::make_pair(name,fi->second));
	metadata->d_keys.erase(d_name);
	metadata->d_onNameChange(d_name,name);
	d_name = name;
}

AntMetaDataType AntMetadata::Key::Type() const {
	return fort::myrmidon::AntMetaDataType(d_default.index());
}

AntStaticValue AntMetadata::DefaultValue(AntMetaDataType type) {
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

AntMetadata::Key::Key(const std::weak_ptr<AntMetadata> & metadata,
                      const std::string & name,
                      const AntStaticValue & defaultValue)
	: d_metadata(metadata)
	, d_name(name)
	, d_default(defaultValue) {
	if ( d_default.index() == std::variant_npos) {
		throw std::runtime_error("Invalid AntStaticValue passed as default value");
	}
}

const AntStaticValue & AntMetadata::Key::DefaultValue() const {
	return d_default;
}

void AntMetadata::Key::SetDefaultValue(const AntStaticValue & value) {
	auto metadata = d_metadata.lock();
	if ( !metadata ) {
		throw DeletedReference<AntMetadata>();
	}
	if ( value.index() != d_default.index() ) {
		metadata->d_onTypeChange(d_name,
		                         AntMetaDataType(d_default.index()),
		                         AntMetaDataType(value.index()));
	}

	metadata->d_onDefaultChange(d_name,d_default,value);

	d_default = value;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
