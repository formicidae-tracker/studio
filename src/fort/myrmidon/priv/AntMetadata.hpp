#pragma once

#include <memory>
#include <string>
#include <map>

namespace fort {
namespace myrmidon {
namespace priv {

class AntMetadata {
public:
	class UniqueColumnList;

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

	class Column {
	public:
		typedef std::shared_ptr<Column>       Ptr;
		typedef std::shared_ptr<const Column> ConstPtr;


		Column(const std::weak_ptr<UniqueColumnList> & list,
		       const std::string & name,
		       Type type);

		static Validity Validate(const std::string & name);

		static std::string FromValue(bool value);
		static std::string FromValue(int32_t value);

		static bool ToBool(const std::string & value);
		static int32_t ToInt(const std::string & value);

		const std::string & Name() const;
		void SetName(const std::string & name);

		Type MetadataType() const;
		void SetMetadataType(Type type);
	private:
		std::weak_ptr<UniqueColumnList> d_list;
		std::string                     d_name;
		Type                            d_type;
	};


	class UniqueColumnList {
	public:
		typedef std::shared_ptr<UniqueColumnList>       Ptr;
		typedef std::shared_ptr<const UniqueColumnList> ConstPtr;

		typedef std::map<std::string,Column::Ptr> ColumnByName;

		static Column::Ptr Create(const Ptr & itself,
		                          const std::string & name,
		                          Type type);

		size_t Count(const std::string & name) const;

		void Delete(const std::string & columnName);

		const ColumnByName & Columns() const;

	private:
		void CheckName(const std::string & name) const;

		friend class Column;
		ColumnByName d_columns;
	};

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
