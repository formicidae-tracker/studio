#pragma once


#include <Eigen/StdVector>
#include <Eigen/Core>

#include <memory>

namespace fort {

namespace myrmidon {


class Metadata;

class Ant {
public:
	typedef std::vector<Ant,Eigen::aligned_allocator<Ant> > List;
	typedef uint32_t ID;

private:
	ID              d_ID;
	Eigen::Vector2d d_position;
	double          d_angle;

	std::shared_ptr<Metadata> d_metadata;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


} // namespace myrmidon

} //namespace fort
