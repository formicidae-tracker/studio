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

		const AntStaticValue & DefaultValue() const;
		void SetDefaultValue( const AntStaticValue & value);

	private:
		std::weak_ptr<AntMetadata> d_metadata;
		std::string                d_name;
		Type                       d_type;
		AntStaticValue             d_default;
	};

	typedef std::map<std::string,Column::Ptr>      ColumnByName;
	typedef std::map<std::string,Column::ConstPtr> ConstColumnByName;

	typedef std::function<void (const std::string &, const std::string &)> NameChangeCallback;
	typedef std::function<void (const std::string &, Type, Type)>          TypeChangeCallback;
	typedef std::function<void (const std::string &,
	                            const AntStaticValue &,
	                            const AntStaticValue &)>                   DefaultChangeCallback;

	static AntMetadata::Validity Validate(Type type, const std::string & value);

	static void CheckType(Type type, const AntStaticValue & data);

	static AntStaticValue FromString(Type type, const std::string & name);

	static Column::Ptr Create(const Ptr & itself,
	                          const std::string & name,
	                          AntMetadata::Type type);

	AntMetadata();

	AntMetadata(const NameChangeCallback & onNameChange,
	            const TypeChangeCallback & onTypeChange,
	            const DefaultChangeCallback & onDefaultChange);

	size_t Count(const std::string & name) const;

	void Delete(const std::string & columnName);

	const ColumnByName & Columns();
	const ConstColumnByName & CColumns() const;

private:
	static AntStaticValue DefaultValue(Type type);

	void CheckName(const std::string & name) const;

	ColumnByName          d_columns;
	NameChangeCallback    d_onNameChange;
	TypeChangeCallback    d_onTypeChange;
	DefaultChangeCallback d_onDefaultChange;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
