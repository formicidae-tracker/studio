#pragma once

#include <memory>
#include <myrmidon/utils/FileSystem.hpp>

#include "ForwardDeclaration.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class ExperimentReadWriter {
public:
	ExperimentReadWriter();
	virtual ~ExperimentReadWriter();

	virtual ExperimentPtr DoOpen(const fs::path & filename) = 0;
	virtual void DoSave(const Experiment & experiment, const fs::path & filename) = 0;


	static ExperimentPtr Open(const fs::path & filename);
	static void Save(const Experiment & experiment, const fs::path & filename);
};

} //namespace priv;

} //namespace myrmidon

} //namespace fort
