#include "AntMetadata.hpp"

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>

#include <stdexcept>
#include <algorithm>

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

const AntMetadata::ColumnByName & AntMetadata::Columns() const {
	return d_columns;
}

size_t AntMetadata::Count(const std::string & name) const {
	return d_columns.count(name);
}

AntMetadata::Validity AntMetadata::Validate(const std::string & name) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

bool AntMetadata::CheckType(Type type, const AntStaticValue & data) {
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
		 [](const std::string & ) ->  AntStaticValue {
			 return false;
		 },
		};
	size_t idx = size_t(type);
	if ( idx >= converters.size() ) {
		throw std::invalid_argument("Unknown AntMetadata::Type value " + std::to_string(idx));
	}
	return converters[idx](value);
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
	d_name = name;
}

AntMetadata::Type AntMetadata::Column::MetadataType() const {
	return d_type;
}

void AntMetadata::Column::SetMetadataType(AntMetadata::Type type){
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
