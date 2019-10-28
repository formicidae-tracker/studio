#pragma once

#include <myrmidon/utils/FileSystem.hpp>

#include <myrmidon/priv/FramePointer.hpp>

class TagInFramePointer {
public:
	TagInFramePointer(const fort::myrmidon::priv::FramePointer::Ptr & frame,
	                  uint32_t tagValue);
	virtual ~TagInFramePointer();

	const fort::myrmidon::priv::FramePointer::Ptr & Frame() const;
	uint32_t TagValue() const;

	fs::path Path() const;

	fs::path Base() const;


private:
	fort::myrmidon::priv::FramePointer::Ptr d_frame;
	uint32_t                                d_tagValue;
};
