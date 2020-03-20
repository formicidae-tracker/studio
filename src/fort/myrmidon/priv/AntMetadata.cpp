#include "AntMetadata.hpp"

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>

#include <stdexcept>
#include <algorithm>

namespace fort {
namespace myrmidon {
namespace priv {

void AntMetadataUniqueColumnList::CheckName(const std::string & name) const {
	auto fi = d_columns.find(name);
	if ( fi != d_columns.cend() ) {
		throw std::invalid_argument("Column name '" + name + "' is already used");
	}
}

AntMetadataColumn::Ptr AntMetadataUniqueColumnList::Create(const Ptr & itself,
                                                           const std::string & name,
                                                           AntMetadata::Type type) {
	itself->CheckName(name);

	auto res = std::make_shared<AntMetadataColumn>(itself,name,type);
	itself->d_columns.insert(std::make_pair(name,res));
	return res;
}

void AntMetadataUniqueColumnList::Delete(const std::string & name) {
	auto fi =  d_columns.find(name);
	if ( fi == d_columns.end() ) {
		throw std::out_of_range("Unmanaged column '" + name + "'");
	}
	d_columns.erase(fi);
}

const AntMetadataUniqueColumnList::ColumnByName & AntMetadataUniqueColumnList::Columns() const {
	return d_columns;
}

size_t AntMetadataUniqueColumnList::Count(const std::string & name) const {
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

const std::string & AntMetadataColumn::Name() const {
	return d_name;
}

void AntMetadataColumn::SetName(const std::string & name) {
	auto list = d_list.lock();
	if ( !list ) {
		throw DeletedReference<AntMetadataUniqueColumnList>();
	}
	list->CheckName(name);
	auto fi = list->d_columns.find(d_name);
	if ( fi == list->d_columns.end() ) {
		throw std::logic_error("column '" + d_name + "' is not managed by its manager");
	}
	list->d_columns.insert(std::make_pair(name,fi->second));
	list->d_columns.erase(d_name);
	d_name = name;
}

AntMetadata::Type AntMetadataColumn::MetadataType() const {
	return d_type;
}

void AntMetadataColumn::SetMetadataType(AntMetadata::Type type){
	d_type = type;
}

AntMetadataColumn::AntMetadataColumn(const std::weak_ptr<AntMetadataUniqueColumnList> & list,
                                     const std::string & name,
                                     AntMetadata::Type type)
	: d_list(list)
	, d_name(name)
	, d_type(type) {
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
