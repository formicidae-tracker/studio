#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

FrameReference::FrameReference(const std::string & path,
                               FrameID frameID,
                               const fort::myrmidon::Time & time)
	: d_path(path)
	, d_id(frameID)
	, d_time(time) {
}


const std::string & FrameReference::Basepath() const {
	return d_path;
}

FrameReference::~FrameReference() { }

const Time & FrameReference::Time() const {
	return d_time;
}


FrameID FrameReference::ID() const {
	return d_id;
}

fs::path FrameReference::Path() const {
	return fs::path(d_path) / std::to_string(d_id);
}


} //namespace priv
} //namespace myrmidon
} //namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p) {
	return out << p.Basepath() << "/" << p.ID();
}
