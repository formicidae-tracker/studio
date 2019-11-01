#pragma once

#include <Eigen/Core>

#include <myrmidon/utils/FileSystem.hpp>
#include <memory>

#include "TagInFramePointer.hpp"


class AntPoseEstimate : public TagInFramePointer {
public:
	typedef std::shared_ptr<AntPoseEstimate> Ptr;
	AntPoseEstimate(const Eigen::Vector2d & head,
	                const Eigen::Vector2d & tail,
	                const fort::myrmidon::priv::FramePointer::ConstPtr & frame,
	                uint32_t tagvalue);
	virtual ~AntPoseEstimate();

	const Eigen::Vector2d & Head() const;
	void SetHead(const Eigen::Vector2d & head);

	const Eigen::Vector2d & Tail() const;
	void SetTail(const Eigen::Vector2d & tail);


private:
	Eigen::Vector2d d_head;
	Eigen::Vector2d d_tail;
};
