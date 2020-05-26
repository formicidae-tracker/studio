#include <fort/myrmidon/Identification.hpp>

namespace fort {
namespace myrmidon {

class CIdentification : protected fort::myrmidon::Identification {
public :
	CIdentification(const fort::myrmidon::Identification::PPtr & pIdentification)
		: fort::myrmidon::Identification(pIdentification) {}

	inline TagID TagValue() const {
		return Identification::TagValue();
	}

	inline AntID TargetAntID() const {
		return Identification::TargetAntID();
	}

	inline const Time::ConstPtr & Start() const {
		return Identification::Start();
	}

	inline const Time::ConstPtr & End() const {
		return Identification::End();
	}

	inline Eigen::Vector2d AntPosition() const {
		return Identification::AntPosition();
	}

	inline double AntAngle() const {
		return Identification::AntAngle();
	}

	inline bool HasUserDefinedAntPosition() const {
		return Identification::HasUserDefinedAntPosition();
	}
};

}
}

#include "RcppCommon.h"

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Identification)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CIdentification)

#include "Rcpp.h"
