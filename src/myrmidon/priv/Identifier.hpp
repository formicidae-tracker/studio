#pragma once

#include <memory>
#include <unordered_map>
#include <set>

#include "../Ant.hpp"

namespace fort {

namespace myrmidon {

namespace pb {
class AntMetadata;
}

namespace priv {

class Ant;
typedef std::shared_ptr<Ant> AntPtr;

typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

class Identification;
typedef std::shared_ptr<Identification> IdentificationPtr;

class FramePointer;
typedef std::shared_ptr<FramePointer> FramePointerPtr;

class Identifier {
public:
	typedef std::shared_ptr<Identifier> Ptr;

	static Ptr Create();

	Ptr Itself() const;

	AntPtr CreateAnt();
	void DeleteAnt(fort::myrmidon::Ant::ID );
	const AntByID & Ants() const;


	IdentificationPtr AddIdentification(fort::myrmidon::Ant::ID id,
	                                    uint32_t tagValue,
	                                    const FramePointerPtr & start,
	                                    const FramePointerPtr & end);

	void LoadAnt(const fort::myrmidon::pb::AntMetadata & pb);

private:
	typedef std::set<fort::myrmidon::Ant::ID> SetOfID;

	Identifier();

	fort::myrmidon::Ant::ID NextAvailableID();
	AntByID d_ants;
	SetOfID d_antIDs;
	bool    d_continuous;

	std::weak_ptr<Identifier> d_itself;
};



} // namespace priv

} // namespace myrmidon

} // namespace fort
