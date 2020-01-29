#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

FrameReference::FrameReference(const std::string & path,
                               FrameID frameID,
                               const fort::myrmidon::Time & time)
	: d_parentPath(path)
	, d_path(d_parentPath / std::to_string(frameID))
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

const fs::path & FrameReference::Path() const {
	return d_path;
}

const fs::path & FrameReference::ParentPath() const {
	return d_parentPath;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p) {
	return out << p.ParentPath().generic_string() << "/" << p.ID();
}
