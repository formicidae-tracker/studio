#pragma once

#include <memory>
#include <string>
#include <map>

namespace fort {
namespace myrmidon {
namespace priv {

class AntMetadata {
public:

	enum class Type {
	                 Bool = 0,
	                 Int,
	                 String,
	};

	enum class Validity {
	                     Ok = 0,
	                     Intermediate = 1,
	                     Invalid = 2,
	};

	static AntMetadata::Validity Validate(const std::string & name);

	static std::string FromValue(bool value);
	static std::string FromValue(int32_t value);

	static bool ToBool(const std::string & value);
	static int32_t ToInt(const std::string & value);

};
class AntMetadataUniqueColumnList;

class AntMetadataColumn {
public:
	typedef std::shared_ptr<AntMetadataColumn>       Ptr;
	typedef std::shared_ptr<const AntMetadataColumn> ConstPtr;


	AntMetadataColumn(const std::weak_ptr<AntMetadataUniqueColumnList> & list,
	                  const std::string & name,
	                  AntMetadata::Type type);

	const std::string & Name() const;
	void SetName(const std::string & name);

	AntMetadata::Type MetadataType() const;
	void SetMetadataType(AntMetadata::Type type);
private:
	std::weak_ptr<AntMetadataUniqueColumnList> d_list;
	std::string                                d_name;
	AntMetadata::Type                          d_type;
};


class AntMetadataUniqueColumnList {
public:
	typedef std::shared_ptr<AntMetadataUniqueColumnList>       Ptr;
	typedef std::shared_ptr<const AntMetadataUniqueColumnList> ConstPtr;

	typedef std::map<std::string,AntMetadataColumn::Ptr> ColumnByName;

	static AntMetadataColumn::Ptr Create(const Ptr & itself,
	                                     const std::string & name,
	                                     AntMetadata::Type type);

	size_t Count(const std::string & name) const;

	void Delete(const std::string & columnName);

	const ColumnByName & Columns() const;

	private:
	void CheckName(const std::string & name) const;

	friend class AntMetadataColumn;
	ColumnByName d_columns;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
