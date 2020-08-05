#pragma once

#include <memory>
#include <fort/myrmidon/utils/FileSystem.hpp>

#include "ForwardDeclaration.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

// A IO Abstraction to Read and Write an Experiment on the filesystem
//
// All the business logic of this library is scattered in small object
// such as <Experiment>, <Identification>, <Ant> ,
// <FramePointer>. This class is here to avoid to scatter the
// marshalling/unmarshalling logic of this object to be scatter over
// all of them.
//
// This approach gives the benefit that the read/write logic is
// completely decoupled from these objects, and we can know supports
// different file type version, or change this logic completely
// without touching any of these objects.
//
//
// ExperimentReadWriter is therefore an abstract interface, where
// differnet file format version could use different logic by
// re-implementing <DoOpen> or <DoSave>.
//
// To save or open an <priv::Experiment> using the default file
// format, simply use the <Open> and <Save> methods.

class ExperimentReadWriter {
public:
	// A Constructor
	ExperimentReadWriter();
	// A Destructor
	virtual ~ExperimentReadWriter();

	// Actually opens a file on the filesystem and unmarshal its data
	// @filename the path to the actual file to open
	// @return the <priv::Experiment::Ptr> saved into the file
	//
	// The choice has been made to not give a std::ifstream on purpose
	// as the file format is free to use this own function if its
	// comes from a third party library. The implementation is allowed
	// to throw std::exception
	virtual ExperimentPtr DoOpen(const fs::path & filename, bool dataLess) = 0;

	// Actually saves an Experiment on the filesystem
	// @experiment the <priv::Experiment> to save
	// @filename the path to the actual file to save to
	//
	// The choice has been made to not give a std::ofstream on purpose
	// as the file format is free to use this own function if its
	// comes from a third party library. The implementation is allowed
	// to throw std::exception
	virtual void DoSave(const Experiment & experiment, const fs::path & filename) = 0;



	// Opens a file with the preferred file format
	// @filename the path to file to open
	// @return the <priv::Experiment::Ptr> saved in the file
	//
	// Opens a file with the preferred file format. This method can
	// throws std:exception
	static ExperimentPtr Open(const fs::path & filename, bool dataLess = false);

	// Saves a file with the preferred file format
	// @experiment the <priv::Experiment> to save
	// @filename the path to file to save
	//
	// Creates a file and saves the Experiment inside it. Truncate it
	// if it is already existing. This ,method can throws
	// std::exception.
	static void Save(const Experiment & experiment, const fs::path & filename);
};

} //namespace priv;
} //namespace myrmidon
} //namespace fort
