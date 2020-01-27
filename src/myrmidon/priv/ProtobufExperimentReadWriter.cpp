#include "ProtobufExperimentReadWriter.hpp"

#include "Experiment.hpp"
#include "Ant.hpp"

#include <myrmidon/Experiment.pb.h>

#include "../utils/ProtobufFileReadWriter.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

ProtobufReadWriter::ProtobufReadWriter() {}
ProtobufReadWriter::~ProtobufReadWriter() {}

Experiment::Ptr ProtobufReadWriter::DoOpen(const fs::path & filename) {
	typedef utils::ProtobufFileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	auto res = Experiment::Create(filename);
	ReadWriter::Read(filename,
	                 [filename](const pb::FileHeader & h) {
		                 if (h.majorversion() != 0 || h.minorversion() != 1 ) {
			                 std::ostringstream os;
			                 os << "unexpected version " << h.majorversion() << "." << h.minorversion()
			                    << " in " << filename
			                    << " can only works with 0.1";
			                 throw std::runtime_error(os.str());
		                 }
	                 },
	                 [&res,filename](const pb::FileLine & line) {
		                 if (line.has_experiment() == true ) {
			                 LoadExperiment(*res, line.experiment());
		                 }

		                 if (line.has_antdata() == true ) {
			                 LoadAnt(*res, line.antdata());
		                 }

		                 if (line.has_trackingdatadirectory() ) {
			                 auto tdd = LoadTrackingDataDirectory(line.trackingdatadirectory(),
			                                                      filename.parent_path());
			                 res->AddTrackingDataDirectory(tdd);
		                 }

	                 });
	return res;
}


void ProtobufReadWriter::DoSave(const Experiment & experiment, const fs::path & filepath) {
	typedef utils::ProtobufFileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	pb::FileHeader h;
	h.set_majorversion(0);
	h.set_minorversion(1);

	std::vector<std::function < void ( pb::FileLine &) > > lines;

	lines.push_back([&experiment](pb::FileLine & line) {
		                SaveExperiment(*(line.mutable_experiment()),experiment);
	                });

	std::vector<fort::myrmidon::Ant::ID> antIDs;
	for (const auto & [ID,a] : experiment.ConstIdentifier().Ants() ) {
		antIDs.push_back(ID);
	}
	std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	                                         fort::myrmidon::Ant::ID b) -> bool {
		                                      return a < b;
	                                      });

	for (const auto & ID : antIDs) {
		lines.push_back([&experiment,ID](pb::FileLine & line) {
			                SaveAnt(*(line.mutable_antdata()),*(experiment.ConstIdentifier().Ants().find(ID)->second));
		                });
	}

	for (const auto & [p,tdd] : experiment.TrackingDataDirectories() ) {
		lines.push_back([tdd](pb::FileLine & line) {
			                SaveTrackingDataDirectory(*line.mutable_trackingdatadirectory(),tdd);
		                });
	}

	ReadWriter::Write(filepath,h,lines);
}


void ProtobufReadWriter::LoadExperiment(Experiment & e,const pb::Experiment & pb) {
	e.SetAuthor(pb.author());
	e.SetName(pb.name());
	e.SetComment(pb.comment());
	static std::map<pb::TagFamily,fort::tags::Family>
		mapping = {
		           {pb::UNSET,fort::tags::Family::Undefined},
		           {pb::TAG16H5,fort::tags::Family::Tag16h5},
		           {pb::TAG25H9,fort::tags::Family::Tag25h9},
		           {pb::TAG36ARTAG,fort::tags::Family::Tag36ARTag},
		           {pb::TAG36H10,fort::tags::Family::Tag36h10},
		           {pb::TAG36H11,fort::tags::Family::Tag36h11},
		           {pb::CIRCLE21H7,fort::tags::Family::Circle21h7},
		           {pb::CIRCLE49H12,fort::tags::Family::Circle49h12},
		           {pb::CUSTOM48H12,fort::tags::Family::Custom48h12},
		           {pb::STANDARD41H12,fort::tags::Family::Standard41h12},
		           {pb::STANDARD52H13,fort::tags::Family::Standard52h13},
	};
	auto fi = mapping.find(pb.tagfamily());
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid protobuf enum value");
	}
	e.SetFamily(fi->second);
	e.SetThreshold(pb.threshold());

}


void ProtobufReadWriter::SaveExperiment(fort::myrmidon::pb::Experiment & pb, const Experiment & e) {
	pb.Clear();
	pb.set_name(e.Name());
	pb.set_author(e.Author());
	pb.set_comment(e.Comment());
	pb.set_threshold(e.Threshold());

	static std::map<fort::tags::Family,pb::TagFamily>
		mapping = {
		           {fort::tags::Family::Undefined,pb::UNSET},
		           {fort::tags::Family::Tag16h5,pb::TAG16H5},
		           {fort::tags::Family::Tag25h9,pb::TAG25H9},
		           {fort::tags::Family::Tag36ARTag,pb::TAG36ARTAG},
		           {fort::tags::Family::Tag36h10,pb::TAG36H10},
		           {fort::tags::Family::Tag36h11,pb::TAG36H11},
		           {fort::tags::Family::Circle21h7,pb::CIRCLE21H7},
		           {fort::tags::Family::Circle49h12,pb::CIRCLE49H12},
		           {fort::tags::Family::Custom48h12,pb::CUSTOM48H12},
		           {fort::tags::Family::Standard41h12,pb::STANDARD41H12},
		           {fort::tags::Family::Standard52h13,pb::STANDARD52H13},
	};
	auto fi = mapping.find(e.Family());
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid Experiment::TagFamily enum value");
	}
	pb.set_tagfamily(fi->second);
}


Time ProtobufReadWriter::LoadTime(const pb::Time & pb, Time::MonoclockID mID) {
	if (pb.monotonic() != 0 ) {
		return Time::FromTimestampAndMonotonic(pb.timestamp(),pb.monotonic(),mID);
	} else {
		return Time::FromTimestamp(pb.timestamp());
	}

}

void ProtobufReadWriter::SaveTime(pb::Time & pb,const Time & t) {
	t.ToTimestamp(*pb.mutable_timestamp());
	if ( t.HasMono() ) {
		pb.set_monotonic(t.MonotonicValue());
	}
}

void ProtobufReadWriter::LoadSegmentIndexer(SegmentIndexer<std::string> & si,
                                            const google::protobuf::RepeatedPtrField<pb::TrackingSegment> & pb,
                                            Time::MonoclockID mID) {
	for(const auto & s : pb) {
		si.Insert(s.frameid(),LoadTime(s.time(),mID),s.data());
	}
}

void ProtobufReadWriter::SaveSegmentIndexer(google::protobuf::RepeatedPtrField<pb::TrackingSegment> * pb,
                                            const SegmentIndexer<std::string> & si) {
	for(const auto & s: si.Segments()) {
		auto spb = pb->Add();
		spb->set_frameid(std::get<0>(s));
		SaveTime(*spb->mutable_time(),std::get<1>(s));
		spb->set_data(std::get<2>(s));
	}
}

MovieSegment::Ptr ProtobufReadWriter::LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & ms,
                                                       const fs::path & base) {
	MovieSegment::ListOfOffset offsets;
	for ( const auto & o : ms.offsets() ) {
		offsets.push_back(std::make_pair(o.movieframeid(),o.offset()));
	}

	return std::make_shared<MovieSegment>(base / ms.path(),
	                                      ms.trackingstart(),
	                                      ms.trackingend(),
	                                      ms.moviestart(),
	                                      ms.movieend(),
	                                      offsets);
}

void ProtobufReadWriter::SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb,
                                          const MovieSegment::Ptr & ms,
                                          const fs::path & base) {
	pb->set_path(fs::relative(ms->MovieFilepath(),base).generic_string());
	pb->set_trackingstart(ms->StartFrame());
	pb->set_trackingend(ms->EndFrame());
	pb->set_moviestart(ms->StartMovieFrame());
	pb->set_movieend(ms->EndMovieFrame());
	for ( const auto & o : ms->Offsets() ) {
		auto pbo = pb->add_offsets();
		pbo->set_movieframeid(o.first);
		pbo->set_offset(o.second);
	}
}

TrackingDataDirectory ProtobufReadWriter::LoadTrackingDataDirectory(const pb::TrackingDataDirectory & pb, const fs::path  & base) {
	TrackingDataDirectory::UID uid = TrackingDataDirectory::GetUID(pb.path());

	auto si = std::make_shared<SegmentIndexer<std::string> >();
	LoadSegmentIndexer(*si,pb.tracking(),uid);
	auto start = LoadTime(pb.startdate(),uid);
	auto end = LoadTime(pb.enddate(),uid);

	MovieSegment::List movies;
	movies.reserve(pb.movies_size());
	for ( const auto & pbMS : pb.movies() ) {
		movies.push_back(LoadMovieSegment(pbMS,base/pb.path()));
	}
	return TrackingDataDirectory(base / pb.path(),
	                             base,
	                             pb.startframe(),
	                             pb.endframe(),
	                             start,
	                             end,
	                             si,
	                             movies);
}

void ProtobufReadWriter::SaveTrackingDataDirectory(pb::TrackingDataDirectory & pb,
                                                   const TrackingDataDirectory & tdd) {
	pb.Clear();
	pb.set_path(tdd.LocalPath().generic_string());
	pb.set_startframe(tdd.StartFrame());
	pb.set_endframe(tdd.EndFrame());
	SaveTime(*pb.mutable_startdate(),tdd.StartDate());
	SaveTime(*pb.mutable_enddate(),tdd.EndDate());
	SaveSegmentIndexer(pb.mutable_tracking(),tdd.TrackingIndex());

	for ( const auto & ms : tdd.MovieSegments() ) {
		SaveMovieSegment(pb.add_movies(),ms,tdd.FilePath());
	}

}


void ProtobufReadWriter::LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb) {
	auto ant = e.Identifier().CreateAnt(pb.id());

	for ( const auto & ident : pb.marker() ) {
		LoadIdentification(e,ant,ident);
	}

	for ( const auto & mpb : pb.measurements() ) {
		ant->SetMeasurement(mpb.name(),mpb.length());
	}

	for ( const auto & mc : pb.shape().capsules() ) {
		ant->AddCapsule(std::make_shared<Capsule>(Eigen::Vector2d(mc.a_x(),mc.a_y()),
		                                          Eigen::Vector2d(mc.b_x(),mc.b_y()),
		                                          mc.a_radius(),
		                                          mc.b_radius()));
	}

}

void ProtobufReadWriter::SaveAnt(fort::myrmidon::pb::AntMetadata & pb, const Ant & ant) {
	pb.Clear();
	pb.set_id(ant.ID());

	for ( const auto & ident : ant.Identifications() ) {
		SaveIdentification(*(pb.add_marker()),*ident);
	}

	for ( const auto & m : ant.Measurements() ) {
		auto mpb = pb.add_measurements();
		mpb->set_name(m.first);
		mpb->set_length(m.second);
	}

	for ( const auto & c : ant.Shape() ) {
		auto cpb = pb.mutable_shape()->add_capsules();
		cpb->set_a_x(c->A().x());
		cpb->set_a_y(c->A().y());
		cpb->set_a_radius(c->RadiusA());
		cpb->set_b_x(c->B().x());
		cpb->set_b_y(c->B().y());
		cpb->set_b_radius(c->RadiusB());
	}

}

void ProtobufReadWriter::LoadIdentification(Experiment & e, const AntPtr & target,
                                            const fort::myrmidon::pb::Identification & pb) {
	Time::ConstPtr start,end;
	if ( pb.has_start() ) {
		start = std::make_shared<Time>(Time::FromTimestamp(pb.start()));
	}
	if ( pb.has_end() ) {
		end = std::make_shared<Time>(Time::FromTimestamp(pb.end()));
	}

	auto res = e.Identifier().AddIdentification(target->ID(),pb.id(),start,end);

	res->SetTagPosition(Eigen::Vector2d(pb.x(),pb.y()),pb.theta());
}

void ProtobufReadWriter::SaveIdentification(fort::myrmidon::pb::Identification & pb,
                                            const Identification & ident) {
	pb.Clear();
	if ( ident.Start() ) {
		ident.Start()->ToTimestamp(*(pb.mutable_start()));
	}
	if ( ident.End() ) {
		ident.End()->ToTimestamp(*(pb.mutable_end()));
	}
	pb.set_x(ident.TagPosition().x());
	pb.set_y(ident.TagPosition().y());
	pb.set_theta(ident.TagAngle());
	pb.set_id(ident.TagValue());
}


}
}
}
