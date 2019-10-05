#pragma once

#include <filesystem>

#include <myrmidon/priv/FramePointer.hpp>

class TagInFramePointer {
public:
	TagInFramePointer(const fort::myrmidon::priv::FramePointer::Ptr & frame,
	                  uint32_t tagValue);
	virtual ~TagInFramePointer();

	const fort::myrmidon::priv::FramePointer::Ptr & Frame() const;
	uint32_t TagValue() const;

	std::filesystem::path Path() const;

	std::filesystem::path Base() const;


private:
	fort::myrmidon::priv::FramePointer::Ptr d_frame;
	uint32_t                                d_tagValue;
};
