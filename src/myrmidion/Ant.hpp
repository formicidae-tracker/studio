#pragma once

#include <vector>
#include <utility>

#include <Eigen/Core>

#include "VisualMarker.hpp"

namespace fort {

namespace myrmidion {




class Ant {
public:
	typedef uint32_t Identifier;

	class AlmostStaticData {
		Identifier                                     d_identifier;
		std::vector<std::pair<uint64_t,VisualMarker> > d_markers;
		std::vector<std::pair<uint64_t,std::string> >  d_metadata;
	};


	const Eigen::Vector2d & Position() const;
	double Angle() const;

	std::string ID() const;

private:
	Eigen::Vector3d d_position;
};


} // namespace myrmidion

} //namespace fort
