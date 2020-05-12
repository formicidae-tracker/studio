#include "Identification.hpp"

#include "priv/Identification.hpp"

namespace fort {
namespace myrmidon {

Identification::Identification(const PPtr & pptr)
	: d_p(pptr) {
}

TagID Identification::TagValue() const {
	return d_p->TagValue();
}

void Identification::SetStart(const Time::ConstPtr & start) {
	d_p->SetStart(start);
}

void Identification::SetEnd(const Time::ConstPtr & end) {
	d_p->SetEnd(end);
}

Time::ConstPtr Identification::Start() const {
	return d_p->Start();
}

Time::ConstPtr Identification::End() const {
	return d_p->End();
}


Eigen::Vector2d Identification::AntPosition() const {
	return d_p->AntPosition();
}

double Identification::AntAngle() const {
	return d_p->AntAngle();
}

bool Identification::HasUserDefinedAntPose() const {
	return d_p->HasUserDefinedAntPose();
}

void Identification::SetUserDefinedAntPose(const Eigen::Vector2d & antPosition,
                                           double antAngle) {
	d_p->SetUserDefinedAntPose(antPosition,antAngle);
}

void Identification::ClearUserDefinedAntPose() {
	d_p->ClearUserDefinedAntPose();
}

OverlappingIdentification::OverlappingIdentification(const priv::Identification & a,
                                                     const priv::Identification & b) noexcept
	: std::runtime_error(Reason(a,b)){
}

std::string OverlappingIdentification::Reason(const priv::Identification & a,
                                              const priv::Identification & b) noexcept {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}


} // namespace fort
} // namespace myrmidon
