#pragma once

#include <myrmidon/utils/FileSystem.hpp>

#include <myrmidon/priv/RawFrame.hpp>

class TagInFramePointer {
public:
	TagInFramePointer(const fort::myrmidon::priv::RawFrame::ConstPtr & frame,
	                  uint32_t tagValue);
	virtual ~TagInFramePointer();

	const fort::myrmidon::priv::RawFrame::ConstPtr & Frame() const;
	uint32_t TagValue() const;

	fs::path Path() const;

	fs::path Base() const;


private:
	fort::myrmidon::priv::RawFrame::ConstPtr d_frame;
	uint32_t                                 d_tagValue;
};
