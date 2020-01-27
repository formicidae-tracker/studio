#pragma once

#include "RawFrame.hpp"
#include "Types.hpp"

namespace fort {

namespace myrmidon {

namespace priv {


class TagInFrameReference {
public:
	TagInFrameReference(const FrameReference & frame,
	                    fort::myrmidon::priv::TagID tagID);
	virtual ~TagInFrameReference();

	const FrameReference & Frame() const;
	TagID TagValue() const;

	fs::path Path() const;

private:
	FrameReference d_reference;
	TagID          d_tagID;
};


} // namespace priv

} // namespace myrmidon

} // namespace fort

// Formats a TagInFrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::TagInFrameReference> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagInFrameReference & p);
