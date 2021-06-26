#pragma once

#include <memory>
#include <string>

#include "ForwardDeclaration.hpp"
#include "ContiguousIDContainer.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class AntShapeType {
public:
	typedef uint32_t                            ID;
	typedef std::shared_ptr<AntShapeType>       Ptr;
	typedef std::shared_ptr<const AntShapeType> ConstPtr;
	typedef DenseMap<ID,Ptr>                    ByID;
	typedef DenseMap<ID,ConstPtr>               ConstByID;

	AntShapeType(ID TypeID, const std::string & name);

	const std::string & Name() const;
	void SetName(const std::string & name);

	ID TypeID() const;

private:
	ID          d_ID;
	std::string d_name;
};

class AntShapeTypeContainer {
public:
	typedef std::shared_ptr<AntShapeTypeContainer>       Ptr;
	typedef std::shared_ptr<const AntShapeTypeContainer> ConstPtr;

	AntShapeType::Ptr Create(const std::string & name,
	                         AntShapeType::ID typeID);

	void Delete(AntShapeType::ID typeID);

	AntShapeType::ByID::const_iterator Find(AntShapeType::ID typeID) const;

	AntShapeType::ByID::const_iterator End() const;

	size_t Count(AntShapeType::ID typeID) const;

	const AntShapeType::ByID & Types();
private:
	AlmostContiguousIDContainer<AntShapeType::ID,AntShapeType> d_container;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
