#include "MetadataColumn.hpp"

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>

#include <stdexcept>

namespace fort {
namespace myrmidon {
namespace priv {

void MetadataColumn::Manager::CheckName(const std::string & name) const {
	auto fi = d_columns.find(name);
	if ( fi != d_columns.cend() ) {
		throw std::invalid_argument("Column name '" + name + "' is already used");
	}
}

MetadataColumn::Ptr MetadataColumn::Manager::Create(const Ptr & itself,
                                                    const std::string & name,
                                                    Type type) {
	itself->CheckName(name);

	auto res = std::make_shared<MetadataColumn>(itself,name,type);
	itself->d_columns.insert(std::make_pair(name,res));
	return res;
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

void MetadataColumn::SetName(const std::string & name) {
	auto manager = d_manager.lock();
	if ( !manager ) {
		throw DeletedReference<Manager>();
	}
	manager->CheckName(name);
	auto fi = manager->d_columns.find(d_name);
	if ( fi == manager->d_columns.end() ) {
		throw std::logic_error("column '" + d_name + "' is not managed by its manager");
	}
	manager->d_columns.insert(std::make_pair(name,fi->second));
	manager->d_columns.erase(d_name);
	d_name = name;
}

MetadataColumn::Type MetadataColumn::MetadataType() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

void MetadataColumn::SetMetadataType(MetadataColumn::Type type){
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

MetadataColumn::MetadataColumn(const std::weak_ptr<Manager> & manager,
                               const std::string & name,
                               MetadataColumn::Type type)
	: d_manager(manager)
	, d_name(name)
	, d_type(type) {
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
