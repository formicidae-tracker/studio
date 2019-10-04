#include "ProtobufExperimentReadWriter.hpp"

#include "Experiment.hpp"
#include "FramePointer.hpp"
#include "Ant.hpp"
#include "FramePointer.hpp"

#include "Experiment.pb.h"

#include "../utils/ProtobufFileReadWriter.hpp"

using namespace fort::myrmidon::priv;

namespace fm = fort::myrmidon;
namespace fs = std::filesystem;

ProtobufReadWriter::ProtobufReadWriter() {}
ProtobufReadWriter::~ProtobufReadWriter() {}

Experiment::Ptr ProtobufReadWriter::DoOpen(const std::filesystem::path & filename) {
	typedef fm::utils::ProtobufFileReadWriter<fm::pb::FileHeader,fm::pb::FileLine> ReadWriter;
	auto res = Experiment::Create(filename);
	ReadWriter::Read(filename,
	                 [filename](const fm::pb::FileHeader & h) {
		                 if (h.majorversion() != 0 || h.minorversion() != 1 ) {
			                 std::ostringstream os;
			                 os << "unexpected version " << h.majorversion() << "." << h.minorversion()
			                    << " in " << filename
			                    << " can only works with 0.1";
			                 throw std::runtime_error(os.str());
		                 }
	                 },
	                 [&res](const fm::pb::FileLine & line) {
		                 if (line.has_experiment() == true ) {
			                 LoadExperiment(*res, line.experiment());
		                 }

		                 if (line.has_antdata() == true ) {
			                 LoadAnt(*res, line.antdata());
		                 }
	                 });
	return res;
}


void ProtobufReadWriter::DoSave(const Experiment & experiment, const std::filesystem::path & filepath) {
	typedef fm::utils::ProtobufFileReadWriter<fm::pb::FileHeader,fm::pb::FileLine> ReadWriter;
	fort::myrmidon::pb::FileHeader h;
	h.set_majorversion(0);
	h.set_minorversion(1);

	std::vector<std::function < void ( fm::pb::FileLine &) > > lines;

	lines.push_back([experiment](fm::pb::FileLine & line) {
		                SaveExperiment(*(line.mutable_experiment()),experiment);
	                });

	std::vector<fort::myrmidon::Ant::ID> antIDs;
	for (const auto & [ID,a] : experiment.Ants() ) {
		antIDs.push_back(ID);
	}
	std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	                                         fort::myrmidon::Ant::ID b) -> bool {
		                                      return a < b;
	                                      });

	for (const auto & ID : antIDs) {
		lines.push_back([experiment,ID](fm::pb::FileLine & line) {
			                SaveAnt(*(line.mutable_antdata()),*(experiment.Ants().find(ID)->second));
		                });
	}

	ReadWriter::Write(filepath,h,lines);
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

	for(const auto & [p,tdd] : e.TrackingDataDirectories() ) {
		SaveTrackingDataDirectory(*(pb.add_datadirectory()),tdd);
	}

}


TrackingDataDirectory ProtobufReadWriter::LoadTrackingDataDirectory(const pb::TrackingDataDirectory & pb) {
	return TrackingDataDirectory(pb.path(),
	                             pb.startframe(),
	                             pb.endframe(),
	                             pb.startdate(),
	                             pb.enddate());
}

void ProtobufReadWriter::SaveTrackingDataDirectory(pb::TrackingDataDirectory & pb,
                                                   const TrackingDataDirectory & tdd) {
	pb.Clear();
	pb.set_path(tdd.Path().generic_string());
	pb.set_startframe(tdd.StartFrame());
	pb.set_endframe(tdd.EndFrame());
	pb.mutable_startdate()->CheckTypeAndMergeFrom(tdd.StartDate());
	pb.mutable_enddate()->CheckTypeAndMergeFrom(tdd.EndDate());
}


void ProtobufReadWriter::LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb) {
	auto ant = e.CreateAnt(pb.id());
	for ( const auto & ident : pb.marker() ) {
		LoadIdentification(e,ant,ident);
	}
}

void ProtobufReadWriter::SaveAnt(fort::myrmidon::pb::AntMetadata & pb, const Ant & ant) {
	pb.Clear();
	pb.set_id(ant.ID());
	for ( const auto & ident : ant.Identifications() ) {
		SaveIdentification(*(pb.add_marker()),*ident);
	}
}

void ProtobufReadWriter::LoadIdentification(Experiment & e, const AntPtr & target,
                                            const fort::myrmidon::pb::Identification & pb) {
	FramePointer::Ptr start,end;
	if ( pb.has_startframe() ) {
		start = LoadFramePointer(pb.startframe());
	}
	if ( pb.has_endframe() ) {
		end = LoadFramePointer(pb.startframe());
	}

	auto res = e.AddIdentification(target->ID(),pb.id(),start,end);

	res->SetTagPosition(Eigen::Vector2d(pb.x(),pb.y()),pb.theta());
}

void ProtobufReadWriter::SaveIdentification(fort::myrmidon::pb::Identification & pb,
                                            const Identification & ident) {
	pb.Clear();
	if ( ident.Start() ) {
		SaveFramePointer(*(pb.mutable_startframe()),*(ident.Start()));
	}
	if ( ident.End() ) {
		SaveFramePointer(*(pb.mutable_endframe()),*(ident.End()));
	}
	pb.set_x(ident.TagPosition().x());
	pb.set_y(ident.TagPosition().y());
	pb.set_theta(ident.TagAngle());
	pb.set_id(ident.TagValue());
}


FramePointer::Ptr ProtobufReadWriter::LoadFramePointer(const fort::myrmidon::pb::FramePointer & pb) {
	if ( pb.path().empty() == true ) {
		return FramePointer::Ptr();
	}
	auto res = std::make_shared<FramePointer>();
	res->Path = std::filesystem::path(pb.path(),std::filesystem::path::generic_format);
	res->Frame = pb.frame();
	res->PathStartDate.CheckTypeAndMergeFrom(pb.pathstartdate());
	return res;
}

void ProtobufReadWriter::SaveFramePointer(fort::myrmidon::pb::FramePointer & pb,
                                          const FramePointer & fp) {
	pb.Clear();
	pb.set_path(fp.Path.generic_string());
	pb.set_frame(fp.Frame);
	pb.mutable_pathstartdate()->CheckTypeAndMergeFrom(fp.PathStartDate);
}
