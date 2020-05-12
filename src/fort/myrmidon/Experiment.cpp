#include "Experiment.hpp"

#include "priv/Experiment.hpp"
#include "priv/AntShapeType.hpp"
#include "priv/Identifier.hpp"

namespace fort {
namespace myrmidon {


Experiment::Ptr Experiment::Open(const std::string & filepath) {
	return std::make_shared<Experiment>(priv::Experiment::Open(filepath));
}

Experiment::ConstPtr Experiment::OpenReadOnly(const std::string & filepath) {
	// its ok to const cast as we cast back as a const
	return std::make_shared<const Experiment>(std::const_pointer_cast<priv::Experiment>(priv::Experiment::OpenReadOnly(filepath)));
}

Experiment::Ptr Experiment::NewFile(const std::string & filepath) {
	return std::make_shared<Experiment>(priv::Experiment::NewFile(filepath));
}

Experiment::Ptr Experiment::Create(const std::string & filepath) {
	return std::make_shared<Experiment>(priv::Experiment::Create(filepath));
}

void Experiment::Save(const std::string & filepath) {
	d_p->Save(filepath);
}

const std::string & Experiment::AbsoluteFilePath() const {
	return d_p->AbsoluteFilePath().string();
}

Space::Ptr Experiment::CreateSpace(const std::string & name) {
	return std::make_shared<Space>(d_p->CreateSpace(name));
}

void Experiment::DeleteSpace(Space::ID spaceID) {
	d_p->DeleteSpace(spaceID);
}

std::map<Space::ID,Space::Ptr> Experiment::Spaces() {
	std::map<Space::ID,Space::Ptr> res;
	for ( const auto & [spaceID, space] : d_p->Spaces() ) {
		res.insert(std::make_pair(spaceID,std::make_shared<Space>(space)));
	}
	return res;
}

std::map<Space::ID,Space::ConstPtr> Experiment::CSpaces() const {
	std::map<Space::ID,Space::ConstPtr> res;
	for ( const auto & [spaceID, space] : d_p->Spaces() ) {
		res.insert(std::make_pair(spaceID,std::make_shared<Space>(space)));
	}
	return res;
}

Ant::Ptr Experiment::CreateAnt() {
	d_p->CreateAnt();
}

std::map<Ant::ID,Ant::Ptr> Experiment::Ants() {
	std::map<Ant::ID,Ant::Ptr> res;
	for ( const auto & [antID, ant] : d_p->Identifier()->Ants() ) {
		res.insert(std::make_pair(antID,std::make_shared<Ant>(ant)));
	}
	return res;
}

std::map<Ant::ID,Ant::ConstPtr> Experiment::CAnts() const {
	std::map<Ant::ID,Ant::ConstPtr> res;
	for ( const auto & [antID, ant] : d_p->Identifier()->Ants() ) {
		res.insert(std::make_pair(antID,std::make_shared<Ant>(ant)));
	}
	return res;
}

Identification::Ptr Experiment::AddIdentification(Ant::ID antID,
                                                  TagID tagID,
                                                  const Time::ConstPtr & start,
                                                  const Time::ConstPtr & end) {
	return std::make_shared<Identification>(priv::Identifier::AddIdentification(d_p->Identifier(),
		      antID,
		      tagID,
		      start,
		      end));
}

void Experiment::DeleteIdentification(const Identification::Ptr & identification) {
	d_p->Identifier()->DeleteIdentification(identification->ToPrivate());
}

bool Experiment::FreeIdentificationRangeAt(Time::ConstPtr & start,
                                           Time::ConstPtr & end,
                                           TagID tagID, const Time & time) const {
	d_p->CIdentifier().FreeRangeContaining(start,end,tagID,time);
}


AntShapeTypeID Experiment::CreateAntShapeType(const std::string & name) {
	return d_p->CreateAntShapeType(name)->TypeID();
}

std::map<AntShapeTypeID,std::string> Experiment::AntShapeTypeNames() const {
	std::map<AntShapeTypeID,std::string> res;
	for ( const auto & [shapeTypeID,shapeType] : d_p->CAntShapeTypes() ) {
		res.insert(std::make_pair(shapeTypeID,shapeType->Name()));
	}
	return res;
}

void Experiment::SetAntShapeTypeName(AntShapeTypeID antShapeTypeID,
                                     const std::string & name) {
	auto fi = d_p->AntShapeTypes().find(antShapeTypeID);
	if ( fi == d_p->AntShapeTypes().end() ) {
		throw std::invalid_argument("unknow AntShapeTypeID " + std::to_string(antShapeTypeID));
	}
	fi->second->SetName(name);
}

void Experiment::DeleteAntShapeType(AntShapeTypeID antShapeTypeID) {
	d_p->DeleteAntShapeType(antShapeTypeID);
}


void Experiment::AddMetadataColumn(const std::string & name,
                                   AntMetadataType type) {
	d_p->AddAntMetadataColumn(name,type);
}

void Experiment::DeleteMetadataColumn(const std::string & name) {
	d_p->DeleteAntMetadataColumn(name);
}

std::map<std::string,AntMetadataType> Experiment::AntMetadataColumns() const {
	std::map<std::string,AntMetadataType> res;
	for ( const auto & [name,column] : d_p->AntMetadataConstPtr()->CColumns() ) {
		res.insert(std::make_pair(name,column->MetadataType()));
	}
	return res;
}

inline priv::AntMetadata::Column & LocateColumn(const priv::Experiment::Ptr & p,
                                                const std::string & name) {
	auto fi = p->AntMetadataPtr()->Columns().find(name);
	if ( fi == p->AntMetadataPtr()->Columns().end() ) {
		throw std::invalid_argument("Unknown AntMetadata Column '" + name + "'");
	}
	return *(fi->second);
}

void Experiment::RenameAntMetadataColumn(const std::string & oldName,
                                         const std::string & newName) {
	LocateColumn(d_p,oldName).SetName(newName);
}

void Experiment::SetAntMetadataColumType(const std::string & name,
                                         AntMetadataType type) {
	LocateColumn(d_p,name).SetMetadataType(type);
}

Experiment::Experiment(const PPtr & pExperiment)
	: d_p(pExperiment) {
}

} //namespace mrymidon
} // namespace fort
