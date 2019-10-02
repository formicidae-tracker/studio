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

class Identifier {
public:
	typedef std::shared_ptr<Identifier> Ptr;

	static Ptr Create();

	Ptr Itself() const;

	AntPtr CreateAnt();
	void DeleteAnt(fort::myrmidon::Ant::ID );
	const AntByID & Ants() const;


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
