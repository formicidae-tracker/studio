#include "ExperimentReadWriter.hpp"

#include "ProtobufExperimentReadWriter.hpp"

#include "Experiment.hpp"

using namespace fort::myrmidon::priv;

ExperimentReadWriter::ExperimentReadWriter() {
}

ExperimentReadWriter::~ExperimentReadWriter() {
}


Experiment::Ptr ExperimentReadWriter::Open(const std::filesystem::path & filename) {
	ProtobufReadWriter pbRW;
	return pbRW.DoOpen(filename);
}

void ExperimentReadWriter::Save(const Experiment & experiment, const std::filesystem::path & filename) {
	ProtobufReadWriter pbRW;
	pbRW.DoSave(experiment,filename);
}


// void Experiment::Load(const std::filesystem::path & filepath) {
// 	ExperimentReadWriter


//	Experiment::Ptr res(new Experiment(filepath));
//	res->Load(filepath);
//	return res;


	// auto basedir = fs::weakly_canonical(d_absoluteFilepath);
	// auto newBasedir = fs::weakly_canonical(filepath);
	// //TODO: should not be an error.
	// if ( basedir.remove_filename() != newBasedir.remove_filename() ) {
	// 	throw std::runtime_error("Changing file directory is not yet supported");
	// }

	// int fd =  open(filepath.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644);
	// if ( fd  < 0 ) {
	// 	throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filepath.string() + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644)");
	// }

	// auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	// file->SetCloseOnDelete(true);
	// auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	// fort::myrmidon::pb::FileHeader h;
	// h.set_majorversion(0);
	// h.set_minorversion(1);

	// if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
	// 	throw std::runtime_error("could not write header message");
	// }

	// fort::myrmidon::pb::FileLine line;

	// line.set_allocated_experiment(const_cast<fort::myrmidon::pb::Experiment*>(&d_experiment));
	// if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
	// 	throw std::runtime_error("could not write experiment data");
	// }
	// line.release_experiment();


	// std::vector<fort::myrmidon::Ant::ID> antIDs;
	// for (const auto & [ID,a] : d_identifier->Ants() ) {
	// 	antIDs.push_back(ID);
	// }
	// std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	//                                          fort::myrmidon::Ant::ID b) -> bool {
	// 	                                      return a < b;
	//                                       });

	// for (const auto & ID : antIDs) {
	// 	line.Clear();
	// 	d_identifier->Ants().find(ID)->second->Encode(*line.mutable_antdata());

	// 	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
	// 		throw std::runtime_error("could not write ant metadata");
	// 	}
	// }




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
