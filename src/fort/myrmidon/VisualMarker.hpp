#pragma once


namespace fort {

namespace myrmidon {

class VisualMarker {
public :
	VisualMarker();
	~VisualMarker();
private:
	uint32_t        d_ID;
	Eigen::Vector3d d_position;
};


}

}
