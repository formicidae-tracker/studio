#include "ExperimentReadWriter.hpp"

#include "ProtobufExperimentReadWriter.hpp"

#include "Experiment.hpp"

using namespace fort::myrmidon::priv;

ExperimentReadWriter::ExperimentReadWriter() {
}

ExperimentReadWriter::~ExperimentReadWriter() {
}


Experiment::Ptr ExperimentReadWriter::Open(const fs::path & filename) {
	ProtobufReadWriter pbRW;
	return pbRW.DoOpen(filename);
}

void ExperimentReadWriter::Save(const Experiment & experiment, const fs::path & filename) {
	ProtobufReadWriter pbRW;
	pbRW.DoSave(experiment,filename);
}


// void Experiment::Load(const std::filesystem::path & filepath) {
// 	ExperimentReadWriter


//	Experiment::Ptr res(new Experiment(filepath));
//	res->Load(filepath);
//	return res;







// 	static std::map<fm::pb::TagFamily,Experiment::TagFamily>
// 		mapping = {
// 		           {fm::pb::UNSET,Experiment::TagFamily::Unset},
// 		           {fm::pb::TAG16H5,Experiment::TagFamily::Tag16h5},
// 		           {fm::pb::TAG25H9,Experiment::TagFamily::Tag25h9},
// 		           {fm::pb::TAG36ARTAG,Experiment::TagFamily::Tag36ARTag},
// 		           {fm::pb::TAG36H10,Experiment::TagFamily::Tag36h10},
// 		           {fm::pb::TAG36H11,Experiment::TagFamily::Tag36h11},
// 		           {fm::pb::CIRCLE21H7,Experiment::TagFamily::Circle21h7},
// 		           {fm::pb::CIRCLE49H12,Experiment::TagFamily::Circle49h12},
// 		           {fm::pb::CUSTOM48H12,Experiment::TagFamily::Custom48h12},
// 		           {fm::pb::STANDARD41H12,Experiment::TagFamily::Standard41h12},
// 		           {fm::pb::STANDARD52H13,Experiment::TagFamily::Standard52h13},
// 	};
// 	auto fi = mapping.find(d_experiment.tagfamily());
// 	if ( fi == mapping.end() ) {
// 		throw std::runtime_error("invalid protobuf enum value");
// 	}
// 	return fi->second;
// }
