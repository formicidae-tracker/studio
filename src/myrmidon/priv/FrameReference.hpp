#pragma once

#include "../Time.hpp"

#include "Types.hpp"
#include "LocatableTypes.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class FrameReference : public RelativelyReferencable {
public:
	typedef std::shared_ptr<FrameReference> Ptr;
	typedef std::shared_ptr<const FrameReference> ConstPtr;

	FrameReference(const std::string & path,
	               FrameID frameID,
	               const fort::myrmidon::Time & Time);

	virtual ~FrameReference();

	const fs::path & ParentPath() const;

	// Returns the <Time> of the Frame
	const fort::myrmidon::Time & Time() const;

	// Returns the <FrameID> of the frame
	FrameID ID() const;

	// A Path uniquely defining the FramePointer
	// @return a fs::path uniquely identifying the Frame
	const fs::path & Path() const override;

private:
	fs::path             d_parentPath,d_path;
	FrameID              d_id;
	fort::myrmidon::Time d_time;
};

} //namespace priv

} //namespace myrmidon

} //namespace fort

// Formats a FrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::FrameReference> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p);
