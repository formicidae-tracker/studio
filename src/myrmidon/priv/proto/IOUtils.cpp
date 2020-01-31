#include "IOUtils.hpp"

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>

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

	res->SetTagPosition(Eigen::Vector2d(pb.x(),pb.y()),pb.theta());
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
	pb->set_x(ident->TagPosition().x());
	pb->set_y(ident->TagPosition().y());
	pb->set_theta(ident->TagAngle());
	pb->set_id(ident->TagValue());
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


// void IOUtils::LoadSegmentIndexer(TrackingDataDirectory::TrackingIndexer & si,
//                                             const google::protobuf::RepeatedPtrField<pb::TrackingSegment> & pb,
//                                             Time::MonoclockID mID) {
// 	for(const auto & s : pb) {
// 		//		si.Insert(s.frameid(),LoadTime(s.time(),mID),s.data());
// 	}
// }

// void IOUtils::SaveSegmentIndexer(google::protobuf::RepeatedPtrField<pb::TrackingSegment> * pb,
//                                             const TrackingDataDirectory::TrackingIndexer & si) {
// 	for(const auto & s: si.Segments()) {
// 		auto spb = pb->Add();
// 		spb->set_frameid(std::get<0>(s));
// 		SaveTime(*spb->mutable_time(),std::get<1>(s));
// 		//		spb->set_data(std::get<2>(s));
// 	}
// }

// MovieSegment::Ptr IOUtils::LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & ms,
//                                                        const fs::path & base) {
// 	MovieSegment::ListOfOffset offsets;
// 	for ( const auto & o : ms.offsets() ) {
// 		offsets.push_back(std::make_pair(o.movieframeid(),o.offset()));
// 	}

// 	return std::make_shared<MovieSegment>(base / ms.path(),
// 	                                      ms.trackingstart(),
// 	                                      ms.trackingend(),
// 	                                      ms.moviestart(),
// 	                                      ms.movieend(),
// 	                                      offsets);
// }

// void IOUtils::SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb,
//                                           const MovieSegment::Ptr & ms,
//                                           const fs::path & base) {
// 	pb->set_path(fs::relative(ms->MovieFilepath(),base).generic_string());
// 	pb->set_trackingstart(ms->StartFrame());
// 	pb->set_trackingend(ms->EndFrame());
// 	pb->set_moviestart(ms->StartMovieFrame());
// 	pb->set_movieend(ms->EndMovieFrame());
// 	for ( const auto & o : ms->Offsets() ) {
// 		auto pbo = pb->add_offsets();
// 		pbo->set_movieframeid(o.first);
// 		pbo->set_offset(o.second);
// 	}
// }

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


void IOUtils::LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb) {
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

void IOUtils::SaveAnt(fort::myrmidon::pb::AntMetadata * pb, const AntConstPtr & ant) {
	pb->Clear();
	pb->set_id(ant->ID());

	for ( const auto & ident : ant->Identifications() ) {
		SaveIdentification(pb->add_marker(),ident);
	}

	for ( const auto & m : ant->Measurements() ) {
		auto mpb = pb->add_measurements();
		mpb->set_name(m.first);
		mpb->set_length(m.second);
	}

	for ( const auto & c : ant->Shape() ) {
		auto cpb = pb->mutable_shape()->add_capsules();
		cpb->set_a_x(c->A().x());
		cpb->set_a_y(c->A().y());
		cpb->set_a_radius(c->RadiusA());
		cpb->set_b_x(c->B().x());
		cpb->set_b_y(c->B().y());
		cpb->set_b_radius(c->RadiusB());
	}

}




} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort