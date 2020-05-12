#pragma once

#include <memory>

#include "Types.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
class Experiment;
} // namespace priv

class Experiment {
public:
	typedef std::shared_ptr<Experiment> Ptr;

	AntShapeTypeID CreateAntShapeType(const std::string & name);

	std::map<AntShapeTypeID,std::string> AntShapeTypeNames() const;

	void SetAntShapeTypeName(AntShapeTypeID antShapeTypeName,
	                         const std::string & name);

	void DeleteAntShapeType(AntShapeTypeID antShapeTypeID);


	void AddMetadataColumn(const std::string & name,
	                       AntMetadataType type);

	void DeleteMetadataColumn(const std::string & name);

	std::map<std::string,AntMetadataType> AntMetadataColumns() const;

	void RenameAntMetadataColumn(const std::string & oldName,
	                             const std::string & newName);

	void SetAntMetadataColumType(const std::string & name,
	                             AntMetadataType type);

	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::Experiment> PPtr;

	// Private implementation constructor
	// @pExperiment opaque pointer to implementation
	//
	// User cannot create an Experiment directly. They must use
	// <Open>, <OpenReadOnly>, <Create> and <NewFile>.
	Experiment(const PPtr & pExperiment);
private:



	PPtr d_p;
};

} //namespace mrymidon
} // namespace fort
