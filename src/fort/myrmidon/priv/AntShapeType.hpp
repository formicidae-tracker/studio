#pragma once

#include <memory>
#include <string>

namespace fort {
namespace myrmidon {
namespace priv {

class AntShapeType {
public:
	typedef uint32_t                            ID;
	typedef std::shared_ptr<AntShapeType>       Ptr;
	typedef std::shared_ptr<const AntShapeType> ConstPtr;

	AntShapeType(ID TypeID, const std::string & name);

	const std::string & Name() const;
	void SetName(const std::string & name);

	ID TypeID() const;

private:
	ID          d_ID;
	std::string d_name;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
