#pragma once

#include <memory>
#include <filesystem>

namespace fort {

namespace myrmidon {

namespace priv {

class Experiment;
typedef std::unique_ptr<Experiment> ExperimentPtr;

class ExperimentReadWriter {
public:
	ExperimentReadWriter();
	virtual ~ExperimentReadWriter();

	virtual ExperimentPtr DoOpen(const std::filesystem::path & filename) = 0;
	virtual void DoSave(const Experiment & experiment, const std::filesystem::path & filename) = 0;


	static ExperimentPtr Open(const std::filesystem::path & filename);
	static void Save(const Experiment & experiment, const std::filesystem::path & filename);
};

} //namespace priv;

} //namespace myrmidon

} //namespace fort
