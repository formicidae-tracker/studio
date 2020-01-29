#include "TagInFrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


TagInFrameReference::TagInFrameReference(const FrameReference & reference,
                                         TagID tagID)
	: d_reference(reference)
	, d_path(d_reference.Path() / std::to_string(tagID))
	, d_tagID(tagID) {
}

TagInFrameReference::~TagInFrameReference() {}

const FrameReference & TagInFrameReference::Frame() const {
	return d_reference;
}

TagID TagInFrameReference::TagValue() const {
	return d_tagID;
}

const fs::path & TagInFrameReference::Path() const {
	return d_path;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagInFrameReference & p) {
	return out << p.Frame() << "/" << p.TagValue();
}
