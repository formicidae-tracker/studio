#include "ProtobufExperimentReadWriter.hpp"

#include "Experiment.hpp"

#include <fcntl.h>

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>

#include "Experiment.pb.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace fort::myrmidon::priv;

namespace fm = fort::myrmidon;
namespace fs = std::filesystem;

ProtobufReadWriter::ProtobufReadWriter() {}
ProtobufReadWriter::~ProtobufReadWriter() {}

Experiment::Ptr ProtobufReadWriter::DoOpen(const std::filesystem::path & filename) {
	auto res = Experiment::Create(filename);
	int fd =  open(filename.c_str(),O_RDONLY | O_BINARY);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filename.string() + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipInputStream>(file.get());


	fort::myrmidon::pb::FileHeader h;
	bool cleanEOF = false;
	if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(),&cleanEOF) ) {
		throw std::runtime_error("could not parse header message in '" + filename.string() + "'");
	}

	if (h.majorversion() != 0 || h.minorversion() != 1 ) {
		std::ostringstream os;
		os << "unexpected version " << h.majorversion() << "." << h.minorversion()
		   << " in " << filename
		   << " can only works with 0.1";
		throw std::runtime_error(os.str());
	}
	fort::myrmidon::pb::FileLine line;

	for (;;) {
		line.Clear();
		if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&line, gunziped.get(), &cleanEOF) ) {
			if ( cleanEOF == true ) {
				break ;
			}
			throw std::runtime_error("Could not read file line");
		}

		if (line.has_experiment() == true ) {
			LoadExperiment(*res, line.experiment());
		}

		if (line.has_antdata() == true ) {
			throw std::runtime_error("not yet implemented");
		}
	}
	return res;
};


void ProtobufReadWriter::DoSave(const Experiment & experiment, const std::filesystem::path & filename) {
}


void ProtobufReadWriter::LoadExperiment(Experiment & e,const fort::myrmidon::pb::Experiment & pb) {
	e.SetAuthor(pb.author());
	e.SetName(pb.name());
	e.SetComment(pb.comment());
	static std::map<fm::pb::TagFamily,Experiment::TagFamily>
		mapping = {
		           {fm::pb::UNSET,Experiment::TagFamily::Unset},
		           {fm::pb::TAG16H5,Experiment::TagFamily::Tag16h5},
		           {fm::pb::TAG25H9,Experiment::TagFamily::Tag25h9},
		           {fm::pb::TAG36ARTAG,Experiment::TagFamily::Tag36ARTag},
		           {fm::pb::TAG36H10,Experiment::TagFamily::Tag36h10},
		           {fm::pb::TAG36H11,Experiment::TagFamily::Tag36h11},
		           {fm::pb::CIRCLE21H7,Experiment::TagFamily::Circle21h7},
		           {fm::pb::CIRCLE49H12,Experiment::TagFamily::Circle49h12},
		           {fm::pb::CUSTOM48H12,Experiment::TagFamily::Custom48h12},
		           {fm::pb::STANDARD41H12,Experiment::TagFamily::Standard41h12},
		           {fm::pb::STANDARD52H13,Experiment::TagFamily::Standard52h13},
	};
	auto fi = mapping.find(pb.tagfamily());
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid protobuf enum value");
	}
	e.SetFamily(fi->second);
	e.SetThreshold(pb.threshold());

	for(const auto & tdd : pb.datadirectory() ) {
		e.AddTrackingDataDirectory(LoadTrackingDataDirectory(tdd));
	}


}


void ProtobufReadWriter::SaveExperiment(fort::myrmidon::pb::Experiment & pb, const Experiment & e) {
	pb.Clear();
	pb.set_name(e.Name());
	pb.set_author(e.Author());
	pb.set_comment(e.Comment());
	pb.set_threshold(e.Threshold());

	static std::map<Experiment::TagFamily,fm::pb::TagFamily>
		mapping = {
		           {Experiment::TagFamily::Unset,fm::pb::UNSET},
		           {Experiment::TagFamily::Tag16h5,fm::pb::TAG16H5},
		           {Experiment::TagFamily::Tag25h9,fm::pb::TAG25H9},
		           {Experiment::TagFamily::Tag36ARTag,fm::pb::TAG36ARTAG},
		           {Experiment::TagFamily::Tag36h10,fm::pb::TAG36H10},
		           {Experiment::TagFamily::Tag36h11,fm::pb::TAG36H11},
		           {Experiment::TagFamily::Circle21h7,fm::pb::CIRCLE21H7},
		           {Experiment::TagFamily::Circle49h12,fm::pb::CIRCLE49H12},
		           {Experiment::TagFamily::Custom48h12,fm::pb::CUSTOM48H12},
		           {Experiment::TagFamily::Standard41h12,fm::pb::STANDARD41H12},
		           {Experiment::TagFamily::Standard52h13,fm::pb::STANDARD52H13},
	};
	auto fi = mapping.find(e.Family());
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid Experiment::TagFamily enum value");
	}
	pb.set_tagfamily(fi->second);



}


TrackingDataDirectory ProtobufReadWriter::LoadTrackingDataDirectory(const pb::TrackingDataDirectory & tdd) {
	TrackingDataDirectory res;
	res.Path = fs::path(tdd.path(),fs::path::generic_format);
	res.StartFrame = tdd.startframe();
	res.EndFrame = tdd.endframe();
	res.StartDate.CheckTypeAndMergeFrom(tdd.startdate());
	res.EndDate.CheckTypeAndMergeFrom(tdd.enddate());
	return res;
}

void ProtobufReadWriter::SaveTrackingDataDirectory(pb::TrackingDataDirectory & pb,
                                                   const TrackingDataDirectory & tdd) {
	pb.Clear();
	pb.set_path(tdd.Path.generic_string());
	pb.set_startframe(tdd.StartFrame);
	pb.set_endframe(tdd.EndFrame);
	pb.mutable_startdate()->CheckTypeAndMergeFrom(tdd.StartDate);
	pb.mutable_enddate()->CheckTypeAndMergeFrom(tdd.EndDate);
}
