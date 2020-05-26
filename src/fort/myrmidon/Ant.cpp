#include "Ant.hpp"

#include "priv/Ant.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

Ant::Ant(const PPtr & pAnt)
	: d_p(pAnt) {
}


Identification::ConstList Ant::CIdentifications() const {
	Identification::ConstList res;
	const auto & idents = d_p->Identifications();
	res.reserve(idents.size());
	for ( const auto & pIdent : idents ) {
		res.push_back(CIdentification(pIdent));
	}
	return res;
}

Identification::List Ant::Identifications() {
	Identification::List res;
	const auto & idents = d_p->Identifications();
	res.reserve(idents.size());
	for ( const auto & pIdent : idents ) {
		res.push_back(Identification(pIdent));
	}
	return res;
}

Ant::ID Ant::AntID() const {
	return d_p->AntID();
}

std::string Ant::FormattedID() const {
	return d_p->FormattedID();
}


const Color & Ant::DisplayColor() const {
	return d_p->DisplayColor();
}

void Ant::SetDisplayColor(const Color & color) {
	d_p->SetDisplayColor(color);
}

Ant::DisplayState Ant::DisplayStatus() const {
	return d_p->DisplayStatus();
}

void Ant::SetDisplayStatus(DisplayState s) {
	d_p->SetDisplayStatus(s);
}

const AntStaticValue & Ant::GetValue(const std::string & name,
                                     const Time & time) const {
	return d_p->GetValue(name,time);
}

void Ant::SetValue(const std::string & name,
                   const AntStaticValue & value,
                   const Time::ConstPtr & time) {
	d_p->SetValue(name,value,time);
}

void Ant::DeleteValue(const std::string & name,
                      const Time::ConstPtr & time) {
	d_p->DeleteValue(name,time);
}

void Ant::AddCapsule(AntShapeTypeID shapeTypeID,
                     const Capsule & capsule) {
	d_p->AddCapsule(shapeTypeID,*capsule.ToPrivate());
}

TypedCapsuleList Ant::Capsules() const {
	TypedCapsuleList res;
	for ( const auto & [shapeTypeID, pCapsule] : d_p->Capsules() ) {
		res.push_back(std::make_pair(shapeTypeID,Capsule(std::make_shared<priv::Capsule>(pCapsule))));
	}
	return res;
}

void Ant::DeleteCapsule(const size_t index) {
	d_p->DeleteCapsule(index);
}

void Ant::ClearCapsules() {
	d_p->ClearCapsules();
}


Identification::ConstList CAnt::CIdentifications() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,CIdentifications);
}

fort::myrmidon::AntID CAnt::AntID() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,AntID);
}


std::string CAnt::FormattedID() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,FormattedID);
}

const Color & CAnt::DisplayColor() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,DisplayColor);
}

Ant::DisplayState CAnt::DisplayStatus() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,DisplayStatus);
}

const AntStaticValue & CAnt::GetValue(const std::string & name,
                                      const Time & time) const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,GetValue,name,time);
}

TypedCapsuleList CAnt::Capsules() const {
	return FORT_MYRMIDON_CONST_HELPER(Ant,Capsules);
}

CAnt::CAnt(const ConstPPtr & pAnt)
	: d_p(pAnt) {
}


} // namespace myrmidon
} // namespace fort
