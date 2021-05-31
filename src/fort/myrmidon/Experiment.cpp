#include "Experiment.hpp"

#include "priv/Experiment.hpp"
#include "priv/TrackingSolver.hpp"
#include "priv/AntShapeType.hpp"
#include "priv/Identifier.hpp"
#include "priv/Measurement.hpp"
#include "priv/TrackingDataDirectory.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {


Experiment Experiment::Open(const std::string & filepath) {
	return Experiment(priv::Experiment::Open(filepath));
}

const Experiment Experiment::OpenDataLess(const std::string & filepath) {
	// its ok to const cast as we cast back as a const
	return Experiment(std::const_pointer_cast<priv::Experiment>(priv::Experiment::OpenDataLess(filepath)));
}

Experiment Experiment::NewFile(const std::string & filepath) {
	return Experiment(priv::Experiment::NewFile(filepath));
}

Experiment Experiment::Create(const std::string & filepath) {
	return Experiment(priv::Experiment::Create(filepath));
}

void Experiment::Save(const std::string & filepath) {
	d_p->Save(filepath);
}

std::string Experiment::AbsoluteFilePath() const {
	return d_p->AbsoluteFilePath().string();
}

Space Experiment::CreateSpace(const std::string & name) {
	return Space(d_p->CreateSpace(name));
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
	for ( const auto & [spaceID, space] : d_p->CSpaces() ) {
		auto pSpace = std::const_pointer_cast<priv::Space>(space);
		res.insert(std::make_pair(spaceID,
		                          std::make_shared<Space>(pSpace)));
	}
	return res;
}

std::map<Space::ID,Space::ConstPtr> Experiment::Spaces() const {
	return CSpaces();
}

std::string Experiment::AddTrackingDataDirectory(Space::ID spaceID,
                                                 const std::string & filepath) {
	auto fi  = d_p->Spaces().find(spaceID);
	if ( fi == d_p->Spaces().end() ) {
		throw std::invalid_argument("Unknown Space::ID " + std::to_string(spaceID));
	}
	auto tdd = priv::TrackingDataDirectory::Open(filepath,d_p->Basedir());
	d_p->AddTrackingDataDirectory(fi->second,tdd);
	return tdd->URI();
}

void Experiment::DeleteTrackingDataDirectory(const std::string & URI) {
	d_p->DeleteTrackingDataDirectory(URI);
}


Ant::Ptr Experiment::CreateAnt() {
	return std::make_shared<Ant>(d_p->CreateAnt());
}

std::map<Ant::ID,Ant::Ptr> Experiment::Ants() {
	std::map<Ant::ID,Ant::Ptr> res;
	for ( const auto & [antID, ant] : d_p->Identifier()->Ants() ) {
		res.insert(std::make_pair(antID,
		                          std::make_shared<Ant>(ant)));
	}
	return res;
}

std::map<Ant::ID,Ant::ConstPtr> Experiment::CAnts() const {
	std::map<Ant::ID,Ant::ConstPtr> res;
	for ( const auto & [antID, ant] : d_p->CIdentifier().CAnts() ) {
		auto pAnt = std::const_pointer_cast<priv::Ant>(ant);
		res.insert(std::make_pair(antID,
		                          std::make_shared<Ant>(pAnt)));
	}
	return res;
}

std::map<Ant::ID,Ant::ConstPtr> Experiment::Ants() const {
	return CAnts();
}


Identification::Ptr Experiment::AddIdentification(Ant::ID antID,
                                                  TagID tagID,
                                                  const Time & start,
                                                  const Time & end) {
	auto i = priv::Identifier::AddIdentification(d_p->Identifier(),
	                                             antID,
	                                             tagID,
	                                             start,
	                                             end);
	return std::make_shared<Identification>(i);
}

void Experiment::DeleteIdentification(const Identification::ConstPtr & identification) {
	d_p->Identifier()->DeleteIdentification(identification->ToPrivate());
}


bool Experiment::FreeIdentificationRangeAt(Time & start,
                                            Time & end,
                                            TagID tagID, const Time & time) const {
	return d_p->CIdentifier().FreeRangeContaining(start,end,tagID,time);
}



const std::string & Experiment::Name() const {
	return d_p->Name();
}

void Experiment::SetName(const std::string & name) {
	d_p->SetName(name);
}

const std::string & Experiment::Author() const {
	return d_p->Author();
}

void Experiment::SetAuthor(const std::string & author) {
	d_p->SetAuthor(author);
}

const std::string & Experiment::Comment() const {
	return d_p->Comment();
}

void Experiment::SetComment(const std::string & comment) {
	d_p->SetComment(comment);
}

fort::tags::Family Experiment::Family() const {
	return d_p->Family();
}

double Experiment::DefaultTagSize() const {
	return d_p->DefaultTagSize();
}

void Experiment::SetDefaultTagSize(double defaultTagSize) {
	d_p->SetDefaultTagSize(defaultTagSize);
}

MeasurementTypeID Experiment::CreateMeasurementType(const std::string & name) {
	return d_p->CreateMeasurementType(name)->MTID();
}

void Experiment::DeleteMeasurementType(MeasurementTypeID mTypeID) {
	d_p->DeleteMeasurementType(mTypeID);
}

void Experiment::SetMeasurementTypeName(MeasurementTypeID mTypeID,
                                        const std::string & name) {
	auto fi = d_p->MeasurementTypes().find(mTypeID);
	if ( fi == d_p->MeasurementTypes().end() ) {
		throw std::invalid_argument("Unknwon measurement type " + std::to_string(mTypeID));
	}
	fi->second->SetName(name);
}


std::map<MeasurementTypeID,std::string> Experiment::MeasurementTypeNames() const {
	std::map<MeasurementTypeID,std::string> res;
	for ( const auto & [mtID,mt] : d_p->CMeasurementTypes() ) {
		res.insert(std::make_pair(mtID,mt->Name()));
	}
	return res;
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
                                   AntMetadataType type,
                                   AntStaticValue defaultValue) {
	auto md = d_p->AddAntMetadataColumn(name,type);
	md->SetDefaultValue(defaultValue);
}

void Experiment::DeleteMetadataColumn(const std::string & name) {
	d_p->DeleteAntMetadataColumn(name);
}


std::map<std::string,std::pair<AntMetadataType,AntStaticValue>>
Experiment::AntMetadataColumns() const {
	std::map<std::string,std::pair<AntMetadataType,AntStaticValue>> res;
	for ( const auto & [name,column] : d_p->AntMetadataConstPtr()->CColumns() ) {
		res.insert(std::make_pair(name,std::make_pair(column->MetadataType(),column->DefaultValue())));
	}
	return res;
}


priv::AntMetadata::Column & LocateColumn(const priv::Experiment::Ptr & p,
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

void Experiment::SetAntMetadataColumnType(const std::string & name,
                                          AntMetadataType type,
                                          AntStaticValue defaultValue) {
	auto & col = LocateColumn(d_p,name);
	if ( col.MetadataType() == type ) {
		col.SetDefaultValue(defaultValue);
		return;
	}
	col.SetMetadataType(type);
	col.SetDefaultValue(defaultValue);
}

Experiment::Experiment(const PPtr & pExperiment)
	: d_p(pExperiment) {
}


TrackingDataDirectoryInfo buildTddInfos(const priv::TrackingDataDirectory::Ptr & tdd) {
	return {
	        .URI = tdd->URI(),
	        .AbsoluteFilePath = tdd->AbsoluteFilePath().string(),
	        .Frames = tdd->EndFrame() - tdd->StartFrame() + 1,
	        .Start  = tdd->StartDate(),
	        .End = tdd->EndDate(),
	};
}


SpaceDataInfo 	buildSpaceInfos( const priv::Space::ConstPtr & space ) {
	SpaceDataInfo res = {.URI = space->URI(),.Name = space->Name(), .Frames = 0 , .Start = Time(), .End = Time()};
	const auto & tdds = space->TrackingDataDirectories();
	if ( tdds.empty() == true ) {
		return res;
	}
	res.Start = tdds.front()->StartDate();
	res.End = tdds.back()->EndDate();
	res.TrackingDataDirectories.reserve(tdds.size());
	for ( const auto & tdd : tdds ) {
		res.TrackingDataDirectories.push_back(buildTddInfos(tdd));
		res.Frames += res.TrackingDataDirectories.back().Frames;
	}
	return res;
}


ExperimentDataInfo Experiment::GetDataInformations() const {
	ExperimentDataInfo res = { .Frames = 0, .Start = Time::Forever(), .End = Time::SinceEver() };
	const auto & spaces = d_p->CSpaces();
	for ( const auto & [spaceID,space] : spaces ) {
		auto sInfo = buildSpaceInfos(space);
		res.Start = std::min(res.Start,sInfo.Start);
		res.End = std::max(res.End,sInfo.End);
		res.Frames += sInfo.Frames;
		res.Spaces[spaceID] = sInfo;
	}

	return res;
}

std::map<AntID,TagID> Experiment::IdentificationsAt(const Time & time,
                                                    bool removeUnidentifiedAnt) const {
	return d_p->CIdentifier().IdentificationsAt(time,removeUnidentifiedAnt);
}

TrackingSolver Experiment::CompileTrackingSolver() const {
	return TrackingSolver(std::make_shared<priv::TrackingSolver>(d_p->Identifier(),
	                                                             d_p->CompileCollisionSolver()));
}



} // namespace mrymidon
} // namespace fort
