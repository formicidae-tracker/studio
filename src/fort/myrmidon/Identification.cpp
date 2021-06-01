#include "Identification.hpp"

#include "priv/Identification.hpp"
#include "priv/Ant.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {


Identification::Identification(const PPtr & pptr)
	: d_p(pptr) {
}


TagID Identification::TagValue() const {
	return d_p->TagValue();
}

AntID Identification::TargetAntID() const {
	return d_p->Target()->AntID();
}


void Identification::SetStart(const Time & start) {
	d_p->SetStart(start);
}

void Identification::SetEnd(const Time & end) {
	d_p->SetEnd(end);
}

Time Identification::Start() const {
	return d_p->Start();
}

Time Identification::End() const {
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

OverlappingIdentification::~OverlappingIdentification() {}

std::string OverlappingIdentification::Reason(const priv::Identification & a,
                                              const priv::Identification & b) noexcept {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}


} // namespace fort
} // namespace myrmidon


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Identification & identification) {
	return out << *identification.d_p;
}
