#pragma once

#include <memory>
#include <string>
#include <map>

#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class AntMetadata {
public:
	typedef std::shared_ptr<AntMetadata>       Ptr;
	typedef std::shared_ptr<const AntMetadata> ConstPtr;

	enum class Type {
	                 Bool = 0,
	                 Int,
	                 Double,
	                 String,
	                 Time,
	};

	enum class Validity {
	                     Valid = 0,
	                     Intermediate = 1,
	                     Invalid = 2,
	};

	class Column {
	public:
		typedef std::shared_ptr<Column>       Ptr;
		typedef std::shared_ptr<const Column> ConstPtr;


		Column(const std::weak_ptr<AntMetadata> & metadata,
		       const std::string & name,
		       Type type);

		const std::string & Name() const;
		void SetName(const std::string & name);

		Type MetadataType() const;
		void SetMetadataType(Type type);
	private:
		std::weak_ptr<AntMetadata> d_metadata;
		std::string                d_name;
		Type                       d_type;
	};

	typedef std::map<std::string,Column::Ptr> ColumnByName;

	typedef std::function<void (const std::string &, const std::string &)> NameChangeCallback;
	typedef std::function<void (const std::string &, Type, Type)>          TypeChangeCallback;

	static AntMetadata::Validity Validate(Type type, const std::string & value);

	static void CheckType(Type type, const AntStaticValue & data);

	static AntStaticValue FromString(Type type, const std::string & name);

	static AntStaticValue DefaultValue(Type type);

	static Column::Ptr Create(const Ptr & itself,
	                          const std::string & name,
	                          AntMetadata::Type type);

	AntMetadata();

	AntMetadata(const NameChangeCallback & onNameChange,
	            const TypeChangeCallback & onTypeChange);

	size_t Count(const std::string & name) const;

	void Delete(const std::string & columnName);

	const ColumnByName & Columns() const;

private:
	void CheckName(const std::string & name) const;

	ColumnByName       d_columns;
	NameChangeCallback d_onNameChange;
	TypeChangeCallback d_onTypeChange;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
