#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


FrameReference::FrameReference()
	: d_parentURI("/")
	, d_URI("/frames/0")
	,  d_id(0) {
}

FrameReference::FrameReference(const std::string & parentURI,
                               priv::FrameID frameID,
                               const fort::Time & time)
	: d_parentURI(parentURI.empty() ? "/" : parentURI)
	, d_URI( (fs::path(d_parentURI) / "frames" / std::to_string(frameID)).generic_string() )
	, d_id(frameID)
	, d_time(time) {
}

FrameReference::~FrameReference() { }

const Time & FrameReference::Time() const {
	return d_time;
}

FrameID FrameReference::FrameID() const {
	return d_id;
}

const std::string & FrameReference::URI() const {
	return d_URI;
}

const std::string & FrameReference::ParentURI() const {
	return d_parentURI;
}

bool FrameReference::operator<(const FrameReference & other) const {
	if ( d_parentURI == other.d_parentURI ) {
		return d_id < other.d_id;
	}
	return d_parentURI < other.d_parentURI;
}

} //namespace priv
} //namespace myrmidon
} //namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p) {
	return out << p.URI();
}
