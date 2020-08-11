#pragma once

#include <fort/myrmidon/priv/ExperimentReadWriter.hpp>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

// Saves Experiment using protocol buffer
//
// This <ExperimentReadWriter> read and saves data using protocol
// buffer.
class ExperimentReadWriter : public priv::ExperimentReadWriter {
public:
	// Constructor
	ExperimentReadWriter();
	// Destructor
	virtual ~ExperimentReadWriter();

	// Implements DoOpen
	virtual ExperimentPtr DoOpen(const fs::path & filename, bool dataLess = false);

	// Implements DoSave
	virtual void DoSave(const Experiment & experiment, const fs::path & filename);

};

} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
