#include "Identification.hpp"

#include "priv/Identification.hpp"
#include "priv/Ant.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {


CIdentification::CIdentification(const ConstPPtr & pIdentification)
	: d_p(pIdentification) {
}


Identification::Identification(const PPtr & pptr)
	: d_p(pptr) {
}

const Identification::PPtr & Identification::ToPrivate() const {
	return d_p;
}

TagID Identification::TagValue() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, TagValue);
}


TagID CIdentification::TagValue() const {
	return d_p->TagValue();
}

AntID Identification::TargetAntID() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, TargetAntID);
}

AntID CIdentification::TargetAntID() const {
	return d_p->Target()->AntID();
}


void Identification::SetStart(const Time & start) {
	d_p->SetStart(start);
}

void Identification::SetEnd(const Time & end) {
	d_p->SetEnd(end);
}

Time Identification::Start() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, Start);
}

Time CIdentification::Start() const {
	return d_p->Start();
}

Time Identification::End() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, End);
}

Time CIdentification::End() const {
	return d_p->End();
}

Eigen::Vector2d Identification::AntPosition() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, AntPosition);
}

Eigen::Vector2d CIdentification::AntPosition() const {
	return d_p->AntPosition();
}

double Identification::AntAngle() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, AntAngle);
}

double CIdentification::AntAngle() const {
	return d_p->AntAngle();
}

bool Identification::HasUserDefinedAntPose() const {
	return FORT_MYRMIDON_CONST_HELPER(Identification, HasUserDefinedAntPose);
}

bool CIdentification::HasUserDefinedAntPose() const {
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
	return out << *(identification.ToPrivate());
}
