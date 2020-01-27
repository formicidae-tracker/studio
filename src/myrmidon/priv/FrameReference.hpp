#pragma once

#include "../Time.hpp"

#include "Types.hpp"

#include <myrmidon/utils/FileSystem.hpp>

namespace fort {

namespace myrmidon {

namespace priv {

class FrameReference {
public:
	FrameReference(const std::string & path,
	               FrameID frameID,
	               const fort::myrmidon::Time & Time);

	virtual ~FrameReference();

	// Returns the basepath to the corresponding frame
	const std::string & Basepath() const;

	// Returns the <Time> of the Frame
	const fort::myrmidon::Time & Time() const;

	// Returns the <FrameID> of the frame
	FrameID ID() const;

	// A Path uniquely defining the FramePointer
	// @return a fs::path uniquely identifying the Frame
	fs::path Path() const;

private:
	std::string          d_path;
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
