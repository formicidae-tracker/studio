#pragma once

#include <Eigen/Core>

#include <filesystem>
#include <memory>

#include <myrmidon/priv/FramePointer.hpp>


class AntPoseEstimate {
public:
	typedef std::shared_ptr<AntPoseEstimate> Ptr;
	AntPoseEstimate(const Eigen::Vector2d & head,
	                const Eigen::Vector2d & tail,
	                const fort::myrmidon::priv::FramePointer::Ptr & frame,
	                uint32_t tagvalue);

	const Eigen::Vector2d & Head() const;
	void SetHead(const Eigen::Vector2d & head);

	const Eigen::Vector2d & Tail() const;
	void SetTail(const Eigen::Vector2d & tail);

	std::filesystem::path Path() const;

	uint32_t TagValue() const;
	const fort::myrmidon::priv::FramePointer::Ptr & Frame() const;


private:
	Eigen::Vector2d d_head;
	Eigen::Vector2d d_tail;

	fort::myrmidon::priv::FramePointer::Ptr d_frame;
	uint32_t d_tagValue;

};
