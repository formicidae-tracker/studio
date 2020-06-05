#pragma once

#include <iterator>
#include <memory>

#include <Eigen/Core>

#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class AntPoseEstimate : public Identifiable {
public :
	typedef std::shared_ptr<AntPoseEstimate>       Ptr;
	typedef std::shared_ptr<const AntPoseEstimate> ConstPtr;

	static std::string BuildURI(const FrameReference & reference,
	                            TagID tid);

	AntPoseEstimate(const FrameReference & reference,
	                TagID tid,
	                const Eigen::Vector2d & positionFromTag,
	                double angleFromTag);

	AntPoseEstimate(const FrameReference & reference,
	                TagID tid,
	                const Eigen::Vector2d & headFromTag,
	                const Eigen::Vector2d & tailFromTag);

	virtual ~AntPoseEstimate();

	const std::string & URI() const override;

	const FrameReference & Reference() const;

	Eigen::Vector2d PositionFromTag() const;
	double AngleFromTag() const;

	TagID  TargetTagID() const;

	template <typename iterator_type>
	static void ComputeMeanPose(Eigen::Vector2d & position,
	                            double & angle,
	                            iterator_type begin,
	                            iterator_type end) {
		position.setZero();
		angle = 0.0;
		double sinAngle(0.0), cosAngle(0.0);
		size_t s = 0;
		for( auto iter = begin; iter != end; ++iter) {
			++s;
			position += (*iter)->PositionFromTag();
			sinAngle += std::sin((*iter)->AngleFromTag());
			cosAngle += std::cos((*iter)->AngleFromTag());
		}
		if ( s == 0 ) {
			return;
		}
		position /= s;
		sinAngle /= s;
		cosAngle /= s;
		angle =  std::atan2(sinAngle,cosAngle);
	};

private:
	double         d_x,d_y,d_angle;
	FrameReference d_reference;
	TagID          d_tid;
	std::string    d_URI;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
