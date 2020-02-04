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


// A ressource identifiable by its URI
//
// <Identifiable> designs a ressource identifiable by <URI>.
class Identifiable {
public:
	virtual ~Identifiable() {};

	// The relative path of the ressource
	//
	// @return a relative path designating the ressource.
	virtual const fs::path & URI() const = 0;

	// Allows Identifiable objects to be comparable
	bool operator<(const Identifiable & other);
};

} //namespace priv
} //namespace myrmidon
} //namespace fort
