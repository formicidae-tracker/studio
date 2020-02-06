#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


FrameReference::FrameReference()
	: d_parentURI("/")
	, d_URI("/frames/0")
	,  d_id(0) {
}

FrameReference::FrameReference(const fs::path & parentURI,
                               FrameID frameID,
                               const fort::myrmidon::Time & time)
	: d_parentURI(parentURI.empty() ? "/" : parentURI)
	, d_URI(d_parentURI / "frames" / std::to_string(frameID))
	, d_id(frameID)
	, d_time(time) {
}

FrameReference::~FrameReference() { }

const Time & FrameReference::Time() const {
	return d_time;
}

FrameID FrameReference::ID() const {
	return d_id;
}

const fs::path & FrameReference::URI() const {
	return d_URI;
}

const fs::path & FrameReference::ParentURI() const {
	return d_parentURI;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p) {
	return out << p.URI().generic_string();
}
