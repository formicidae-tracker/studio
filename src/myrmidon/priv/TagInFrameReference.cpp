#include "TagInFrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


TagInFrameReference::TagInFrameReference(const FrameReference & reference,
                                         TagID tagID)
	: d_reference(reference)
	, d_URI(d_reference.URI() / "tags" / std::to_string(tagID))
	, d_tagID(tagID) {
}

TagInFrameReference::~TagInFrameReference() {}

const FrameReference & TagInFrameReference::Frame() const {
	return d_reference;
}

TagID TagInFrameReference::TagValue() const {
	return d_tagID;
}

const fs::path & TagInFrameReference::URI() const {
	return d_URI;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagInFrameReference & p) {
	return out << p.Frame() << "/tags/" << p.TagValue();
}
