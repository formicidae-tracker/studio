#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include "DeletedReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

// A ressource locatable on the filesystem
//
// <FileSystemLocatable> designs a ressource that is absolutely
// locatable on the filesystem.
class FileSystemLocatable {
public:
	typedef std::shared_ptr<FileSystemLocatable> Ptr;
	virtual ~FileSystemLocatable() {};

	// The absolute path of the file
	//
	// @return an absolute path to the file.
	virtual const fs::path & AbsoluteFilePath() const = 0;
};


// A ressource referencable to an Experiment
//
// <RelativelyReferencable> designs a ressource relatively to an
// ExperimentRoot. It is not necessarly a file on the filesystem.
class RelativelyReferencable {
public:
	virtual ~RelativelyReferencable() {};

	// The relative path of the ressource
	//
	// @return a relative path designating the ressource.
	virtual const fs::path & Path() const = 0;
};

} //namespace priv
} //namespace myrmidon
} //namespace fort
