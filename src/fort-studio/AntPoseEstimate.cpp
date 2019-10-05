#include "AntPoseEstimate.hpp"


AntPoseEstimate::AntPoseEstimate(const Eigen::Vector2d & head,
                                 const Eigen::Vector2d & tail,
                                 const fort::myrmidon::priv::FramePointer::Ptr & frame,
                                 uint32_t tagValue)
	: d_head(head)
	, d_tail(tail)
	, d_frame(frame)
	, d_tagValue(tagValue) {
}


const Eigen::Vector2d & AntPoseEstimate::Head() const {
	return d_head;
}

void AntPoseEstimate::SetHead(const Eigen::Vector2d & head) {
	d_head = head;
}

const Eigen::Vector2d & AntPoseEstimate::Tail() const {
	return d_tail;
}

void AntPoseEstimate::SetTail(const Eigen::Vector2d & tail) {
	d_tail = tail;
}

std::filesystem::path AntPoseEstimate::Path() const {
	std::ostringstream os;
	os << d_tagValue;
	return d_frame->FullPath() / os.str();
}
