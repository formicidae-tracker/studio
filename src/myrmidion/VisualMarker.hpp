#pragma once


namespace fort {

namespace myrmidion {

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
