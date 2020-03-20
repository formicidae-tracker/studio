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

std::string AntMetadata::FromValue(bool value) {
	return value ?  "TRUE" :  "FALSE";
}

std::string AntMetadata::FromValue(int32_t value) {
	return std::to_string(value);
}

bool AntMetadata::ToBool(const std::string & value) {
	std::string lowered(value);
	std::transform(lowered.begin(),
	               lowered.end(),
	               lowered.begin(),
	               [](unsigned char c ) -> unsigned char { return std::tolower(c);});
	if ( value.empty() == true || lowered == "false" ) {
		return false;
	}
	return true;
}

int32_t AntMetadata::ToInt(const std::string & value) {
	try {
		return std::stoi(value);
	} catch ( const std::exception & e ) {
		return 0;
	}
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
