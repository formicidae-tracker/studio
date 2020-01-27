#pragma once

#include <myrmidon/utils/FileSystem.hpp>

#include <myrmidon/priv/RawFrame.hpp>

#include <myrmidon/priv/Types.hpp>

class TagInFramePointer {
public:
	TagInFramePointer(const fort::myrmidon::priv::RawFrame::ConstPtr & frame,
	                  fort::myrmidon::priv::TagID tagValue);
	virtual ~TagInFramePointer();

	const fort::myrmidon::priv::RawFrame::ConstPtr & Frame() const;
	fort::myrmidon::priv::TagID TagValue() const;

	fs::path Path() const;

	fs::path Base() const;


private:
	fort::myrmidon::priv::RawFrame::ConstPtr d_frame;
	fort::myrmidon::priv::TagID              d_tagValue;
};
