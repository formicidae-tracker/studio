#pragma once

#include <map>
#include <memory>

namespace fort {
namespace myrmidon {

namespace priv {
class Zone;
}

class Zone {
public:
	typedef uint32_t                    ID;
	typedef std::shared_ptr<Zone>       Ptr;
	typedef std::shared_ptr<const Zone> ConstPtr;
	typedef std::map<ID,Ptr>            ByID;
	typedef std::map<ID,ConstPtr>       ConstByID;

	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Zone> PPtr;

	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Zone directly. They must be build and
	// accessed from <Space>.
	Zone(const PPtr & pZone);
private:
	PPtr d_p;
};


} // namespace myrmidon
} // namespace fort
