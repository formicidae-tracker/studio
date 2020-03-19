#include "MetadataColumn.hpp"

#include <fort/myrmidon/utils/NotYetImplemented.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

MetadataColumn::Ptr MetadataColumn::Manager::Create(const Ptr & itself,
                                                           const std::string & name,
                                                           Type type) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

void MetadataColumn::Manager::Delete(const std::string & columnName) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

const MetadataColumn::Manager::ColumnByName & MetadataColumn::Manager::Columns() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}


MetadataColumn::Validity MetadataColumn::Validate(const std::string & name) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

std::string MetadataColumn::FromValue(bool value) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

std::string MetadataColumn::FromValue(int32_t value) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

bool MetadataColumn::ToBool(const std::string & value) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

int32_t MetadataColumn::ToInt(const std::string & value) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

const std::string & MetadataColumn::Name() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

void MetadataColumn::SetName(const std::string & name) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

MetadataColumn::Type MetadataColumn::MetadataType() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

void MetadataColumn::SetMetadataType(MetadataColumn::Type type) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

MetadataColumn::MetadataColumn(const std::weak_ptr<Manager> & manager,
                               const std::string & name,
                               MetadataColumn::Type type) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
