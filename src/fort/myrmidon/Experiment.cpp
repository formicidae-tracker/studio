#include "Experiment.hpp"

#include "priv/Experiment.hpp"
#include "priv/AntShapeType.hpp"

namespace fort {
namespace myrmidon {


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
