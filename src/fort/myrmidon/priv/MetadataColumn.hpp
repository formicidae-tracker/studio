#pragma once

#include <memory>
#include <string>
#include <map>

namespace fort {
namespace myrmidon {
namespace priv {

class MetadataColumn {
public:
	typedef std::shared_ptr<MetadataColumn>       Ptr;
	typedef std::shared_ptr<const MetadataColumn> ConstPtr;

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

	class Manager {
	public:
		typedef std::shared_ptr<Manager>       Ptr;
		typedef std::shared_ptr<const Manager> ConstPtr;

		typedef std::map<std::string,MetadataColumn::Ptr> ColumnByName;

		static MetadataColumn::Ptr Create(const Ptr & itself,
		                                  const std::string & name,
		                                  Type type);

		void Delete(const std::string & columnName);

		const ColumnByName & Columns() const;

	private:
		void CheckName(const std::string & name) const;

		friend class MetadataColumn;
		ColumnByName d_columns;
	};

	MetadataColumn(const std::weak_ptr<Manager> & manager,
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


	std::weak_ptr<Manager> d_manager;
	std::string            d_name;
	Type                   d_type;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
