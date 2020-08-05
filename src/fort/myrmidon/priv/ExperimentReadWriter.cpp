#include "ExperimentReadWriter.hpp"

#include <fort/myrmidon/priv/proto/ExperimentReadWriter.hpp>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

ExperimentReadWriter::ExperimentReadWriter() {
}

ExperimentReadWriter::~ExperimentReadWriter() {
}


Experiment::Ptr ExperimentReadWriter::Open(const fs::path & filename, bool dataLess) {
	proto::ExperimentReadWriter pbRW;
	return pbRW.DoOpen(filename,dataLess);
}

void ExperimentReadWriter::Save(const Experiment & experiment, const fs::path & filename) {
	proto::ExperimentReadWriter pbRW;
	pbRW.DoSave(experiment,filename);
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
