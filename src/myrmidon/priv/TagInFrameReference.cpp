#include "TagInFrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


TagInFrameReference::TagInFrameReference(const FrameReference & reference,
                                         TagID tagID)
	: d_reference(reference)
	, d_tagID(tagID) {
}

TagInFrameReference::~TagInFrameReference() {}

const FrameReference & TagInFrameReference::Frame() const {
	return d_reference;
}

TagID TagInFrameReference::TagValue() const {
	return d_tagID;
}

fs::path TagInFrameReference::Path() const {
	return d_reference.Path() / std::to_string(d_tagID);
}


} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagInFrameReference & p) {
	return out << p.Frame() << "/" << p.TagValue();
}
