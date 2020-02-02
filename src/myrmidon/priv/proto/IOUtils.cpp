#include "IOUtils.hpp"

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>
#include <myrmidon/priv/Shape.hpp>
#include <myrmidon/priv/TagCloseUp.hpp>

#include <myrmidon/utils/Checker.hpp>


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


Time IOUtils::LoadTime(const pb::Time & pb, Time::MonoclockID mID) {
	if (pb.monotonic() != 0 ) {
		return Time::FromTimestampAndMonotonic(pb.timestamp(),pb.monotonic(),mID);
	} else {
		return Time::FromTimestamp(pb.timestamp());
	}

}

void IOUtils::SaveTime(pb::Time * pb,const Time & t) {
	t.ToTimestamp(pb->mutable_timestamp());
	if ( t.HasMono() ) {
		pb->set_monotonic(t.MonotonicValue());
	}
}



void IOUtils::LoadIdentification(Experiment & e, const AntPtr & target,
                                 const fort::myrmidon::pb::Identification & pb) {
	Time::ConstPtr start,end;
	if ( pb.has_start() ) {
		start = std::make_shared<Time>(Time::FromTimestamp(pb.start()));
	}
	if ( pb.has_end() ) {
		end = std::make_shared<Time>(Time::FromTimestamp(pb.end()));
	}

	auto res = e.Identifier().AddIdentification(target->ID(),pb.id(),start,end);
	Eigen::Vector2d pos;
	LoadVector(pos,pb.position());
	res->SetTagPosition(pos,pb.theta());
}

void IOUtils::SaveIdentification(fort::myrmidon::pb::Identification * pb,
                                 const Identification::ConstPtr & ident) {
	pb->Clear();
	if ( ident->Start() ) {
		ident->Start()->ToTimestamp(pb->mutable_start());
	}
	if ( ident->End() ) {
		ident->End()->ToTimestamp(pb->mutable_end());
	}
	SaveVector(pb->mutable_position(),ident->TagPosition());
	pb->set_theta(ident->TagAngle());
	pb->set_id(ident->TagValue());
}


Capsule::Ptr IOUtils::LoadCapsule(const pb::Capsule & pb) {
	Eigen::Vector2d a,b;
	LoadVector(a,pb.a());
	LoadVector(b,pb.b());
	return std::make_shared<Capsule>(a,b,
	                                 pb.a_radius(),
	                                 pb.b_radius());
}

void IOUtils::SaveCapsule(pb::Capsule * pb,const Capsule::ConstPtr & capsule) {
	SaveVector(pb->mutable_a(),capsule->A());
	SaveVector(pb->mutable_b(),capsule->B());
	pb->set_a_radius(capsule->RadiusA());
	pb->set_b_radius(capsule->RadiusB());
}


void IOUtils::LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb) {
	auto ant = e.Identifier().CreateAnt(pb.id());

	for ( const auto & ident : pb.identifications() ) {
		LoadIdentification(e,ant,ident);
	}

	for ( const auto & mpb : pb.measurements() ) {
		ant->SetMeasurement(mpb.name(),mpb.lengthmm());
	}

	for ( const auto & mc : pb.shape().capsules() ) {
		ant->AddCapsule(LoadCapsule(mc));
	}

}

void IOUtils::SaveAnt(fort::myrmidon::pb::AntMetadata * pb, const AntConstPtr & ant) {
	pb->Clear();
	pb->set_id(ant->ID());

	for ( const auto & ident : ant->Identifications() ) {
		SaveIdentification(pb->add_identifications(),ident);
	}

	for ( const auto & m : ant->Measurements() ) {
		auto mpb = pb->add_measurements();
		mpb->set_name(m.first);
		mpb->set_lengthmm(m.second);
	}

	for ( const auto & c : ant->Shape() ) {
		SaveCapsule(pb->mutable_shape()->add_capsules(),c);
	}

}



void IOUtils::LoadExperiment(Experiment & e,
                             const pb::Experiment & pb) {
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

	for ( const auto tddRelPath : pb.trackingdatadirectories() ) {
		auto tdd = TrackingDataDirectory::Open(e.Basedir() / tddRelPath, e.Basedir());
		e.AddTrackingDataDirectory(tdd);
	}
}


void IOUtils::SaveExperiment(fort::myrmidon::pb::Experiment * pb, const Experiment & e) {
	pb->Clear();
	pb->set_name(e.Name());
	pb->set_author(e.Author());
	pb->set_comment(e.Comment());
	pb->set_threshold(e.Threshold());

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
	pb->set_tagfamily(fi->second);

	for ( const auto & [p,tdd] : e.TrackingDataDirectories() ) {
		pb->add_trackingdatadirectories(tdd->URI().generic_string());
	}
}

FrameReference IOUtils::LoadFrameReference(const pb::TimedFrame & pb,
                                           const fs::path & parentURI,
                                           Time::MonoclockID monoID) {
	return FrameReference(parentURI,pb.frameid(),LoadTime(pb.time(),monoID));
}

void IOUtils::SaveFrameReference(pb::TimedFrame * pb,
                                 const FrameReference & ref) {
		pb->set_frameid(ref.ID());
		SaveTime(pb->mutable_time(),ref.Time());
}

TrackingDataDirectory::TrackingIndex::Segment
IOUtils::LoadTrackingIndexSegment(const pb::TrackingSegment & pb,
                                  const fs::path & parentURI,
                                  Time::MonoclockID monoID) {
	return std::make_pair(LoadFrameReference(pb.frame(),parentURI,monoID),
	                      pb.filename());
}

void IOUtils::SaveTrackingIndexSegment(pb::TrackingSegment * pb,
                                       const TrackingDataDirectory::TrackingIndex::Segment & si) {
	SaveFrameReference(pb->mutable_frame(),si.first);
	pb->set_filename(si.second);
}


MovieSegment::Ptr
IOUtils::LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & pb,
                          const fs::path & parentAbsoluteFilePath,
                          const fs::path & parentURI) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(parentAbsoluteFilePath);

	MovieSegment::ListOfOffset offsets;
	for ( const auto & o : pb.offsets() ) {
		offsets.push_back(std::make_pair(o.movieframeid(),o.offset()));
	}

	return std::make_shared<MovieSegment>(pb.id(),
	                                      parentAbsoluteFilePath / pb.path(),
	                                      parentURI,
	                                      pb.trackingstart(),
	                                      pb.trackingend(),
	                                      pb.moviestart(),
	                                      pb.movieend(),
	                                      offsets);
}

void IOUtils::SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb,
                               const MovieSegment::ConstPtr & ms,
                               const fs::path & parentAbsoluteFilePath) {

	if (parentAbsoluteFilePath.is_absolute() == false ) {
		throw std::invalid_argument("parentAbsoluteFilePath:'"
		                            + parentAbsoluteFilePath.string()
		                            + "' is not an absolute path");
	}
	pb->Clear();

	pb->set_id(ms->ID());
	pb->set_path(fs::relative(ms->AbsoluteFilePath(),parentAbsoluteFilePath).generic_string());
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

TagCloseUp::ConstPtr IOUtils::LoadTagCloseUp(const pb::TagCloseUp & pb,
                                             const fs::path & absoluteBasedir,
                                             std::function<FrameReference (FrameID)> resolver) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteBasedir);

	TagCloseUp::Vector2dList corners;
	if ( pb.corners_size() != 4 ) {
		throw std::invalid_argument("protobuf message does not contains 4 corners");
	}
	corners.resize(4);
	for(size_t i = 0; i < 4; ++i ) {
		LoadVector(corners[i],pb.corners(i));
	}
	Eigen::Vector2d position;
	LoadVector(position,pb.position());

	return std::make_shared<TagCloseUp>(absoluteBasedir / pb.imagepath(),
	                                    resolver(pb.frameid()),
	                                    pb.value(),
	                                    position,
	                                    pb.angle(),
	                                    corners);
}

void IOUtils::SaveTagCloseUp(pb::TagCloseUp * pb,
                             const TagCloseUp::ConstPtr & tcu,
                             const fs::path & absoluteBasedir) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteBasedir);

	pb->Clear();

	pb->set_frameid(tcu->Frame().ID());
	pb->set_imagepath(fs::relative(tcu->AbsoluteFilePath(),absoluteBasedir).generic_string());
	SaveVector(pb->mutable_position(),tcu->TagPosition());
	pb->set_angle(tcu->TagAngle());
	pb->set_value(tcu->TagValue());
	for (const auto & c : tcu->Corners()) {
		SaveVector(pb->add_corners(),c);
	}

}


// TrackingDataDirectory IOUtils::LoadTrackingDataDirectory(const pb::TrackingDataDirectory & pb, const fs::path  & base) {
// 	// TrackingDataDirectory::UID uid = TrackingDataDirectory::GetUID(pb.path());

// 	// auto si = std::make_shared<TrackingDataDirectory::TrackingIndexer>();
// 	// LoadSegmentIndexer(*si,pb.tracking(),uid);
// 	// auto start = LoadTime(pb.startdate(),uid);
// 	// auto end = LoadTime(pb.enddate(),uid);

// 	// MovieSegment::List movies;
// 	// movies.reserve(pb.movies_size());
// 	// for ( const auto & pbMS : pb.movies() ) {
// 	// 	movies.push_back(LoadMovieSegment(pbMS,base/pb.path()));
// 	// }
// 	// return TrackingDataDirectory(base / pb.path(),
// 	//                              base,
// 	//                              pb.startframe(),
// 	//                              pb.endframe(),
// 	//                              start,
// 	//                              end,
// 	//                              si,
// 	//                              movies);
// 	throw std::runtime_error("foo");
// }

// void IOUtils::SaveTrackingDataDirectory(pb::TrackingDataDirectory & pb,
//                                                    const TrackingDataDirectory & tdd) {
// 	pb.Clear();
// 	// pb.set_path(tdd.URI().generic_string());
// 	// pb.set_startframe(tdd.StartFrame());
// 	// pb.set_endframe(tdd.EndFrame());
// 	// SaveTime(*pb.mutable_startdate(),tdd.StartDate());
// 	// SaveTime(*pb.mutable_enddate(),tdd.EndDate());
// 	// SaveSegmentIndexer(pb.mutable_tracking(),tdd.TrackingIndex());

// 	// for ( const auto & ms : tdd.MovieSegments() ) {
// 	// 	SaveMovieSegment(pb.add_movies(),ms,tdd.AbsoluteFilePath());
// 	// }

// }






} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
