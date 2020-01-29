#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include "DeletedReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

// A ressource locatable on the filesystem
//
// <FileSystemLocatable> design a ressource that is absolutely
// locatable on the filesystem.
class FileSystemLocatable {
public:
	typedef std::shared_ptr<FileSystemLocatable> Ptr;
	virtual ~FileSystemLocatable() {};

	virtual const fs::path & AbsoluteFilePath() const = 0;
};


// A ressource referencable to
class RelativelyReferencable {
public:
	virtual ~RelativelyReferencable() {};

	virtual const fs::path & Path() const = 0;
};

} //namespace priv
} //namespace myrmidon
} //namespace fort
