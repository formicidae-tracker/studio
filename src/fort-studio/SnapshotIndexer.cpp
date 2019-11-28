#include "SnapshotIndexer.hpp"

#include <apriltag/tag16h5.h>
#include <apriltag/tag25h9.h>
#include <apriltag/tag36h11.h>
#include <apriltag/tagCircle21h7.h>
#include <apriltag/tagCircle49h12.h>
#include <apriltag/tagCustom48h12.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tagStandard52h13.h>
#include <fort-tags/tag36ARTag.h>
#include <fort-tags/tag36h10.h>

#include <cctype>

#include <regex>

#include <QtConcurrent/QtConcurrent>
#include <QImage>

#include <myrmidon/priv/RawFrame.hpp>
#include <myrmidon/utils/ProtobufFileReadWriter.hpp>

#include "SnapshotCache.pb.h"

SnapshotIndexer::SnapshotIndexer(const fort::myrmidon::priv::TrackingDataDirectory & tdd,
                                 const fs::path & basedir,
                                 fort::myrmidon::priv::Experiment::TagFamily family,
                                 uint8_t threshold,
                                 QObject * parent)
	: QObject(parent)
	, d_tdd(tdd)
	, d_basedir(basedir)
	, d_familyValue(family) {
	struct FamilyInterface {
		typedef apriltag_family_t* (*Constructor) ();
		typedef void (*Destructor) (apriltag_family_t *);
		Constructor c;
		Destructor  d;
	};
	using namespace fort::myrmidon::priv;
	static std::unordered_map<Experiment::TagFamily,FamilyInterface>  familyFactory = {
		 {Experiment::TagFamily::Tag16h5,{.c = tag16h5_create, .d = tag16h5_destroy}},
		 {Experiment::TagFamily::Tag25h9,{.c =tag25h9_create, .d=tag25h9_destroy}},
		 {Experiment::TagFamily::Tag36h10,{.c =tag36h10_create, .d=tag36h10_destroy}},
		 {Experiment::TagFamily::Tag36h11,{.c =tag36h11_create, .d=tag36h11_destroy}},
		 {Experiment::TagFamily::Tag36ARTag,{.c =tag36ARTag_create, .d=tag36ARTag_destroy}},
		 {Experiment::TagFamily::Circle21h7,{.c =tagCircle21h7_create, .d=tagCircle21h7_destroy}},
		 {Experiment::TagFamily::Circle49h12,{.c =tagCircle49h12_create, .d=tagCircle49h12_destroy}},
		 {Experiment::TagFamily::Custom48h12,{.c =tagCustom48h12_create, .d=tagCustom48h12_destroy}},
		 {Experiment::TagFamily::Standard41h12,{.c =tagStandard41h12_create, .d=tagStandard41h12_destroy}},
		 {Experiment::TagFamily::Standard52h13,{.c =tagStandard52h13_create, .d=tagStandard52h13_destroy}},
	};


	auto f = familyFactory.find(family);
	if ( f == familyFactory.end() ) {
		std::ostringstream os;
		os << "Unsupported family : " << family;
		throw std::runtime_error(os.str());
	}
	d_family = std::shared_ptr<apriltag_family_t>(f->second.c(),f->second.d);
	d_detector = std::shared_ptr<apriltag_detector_t>(apriltag_detector_create(),apriltag_detector_destroy);

	apriltag_detector_add_family(d_detector.get(),d_family.get());
	d_detector->nthreads = 1;
	d_detector->quad_decimate = 1.0;
	d_detector->quad_sigma = 0.0;
	d_detector->refine_edges = 0;
	d_detector->debug = false;
	d_detector->qtp.min_cluster_pixels = 25;
	d_detector->qtp.max_nmaxima = 10;
	d_detector->qtp.critical_rad = 10.0 * M_PI / 180.0;
	d_detector->qtp.max_line_fit_mse = 10.0;
	d_detector->qtp.min_white_black_diff = threshold;
	d_detector->qtp.deglitch = 0;

}

SnapshotIndexer::~SnapshotIndexer() {
	cancel();
}

void SnapshotIndexer::Process(ImageToProcess & tp) {
	auto fi = d_cache.find(tp.RelativeImagePath);
	if ( fi != d_cache.end() ) {
		tp.Results = fi->second;
		return;
	}


	auto path = tp.Basedir / tp.Frame->Basepath() / tp.RelativeImagePath;
	QImage image(path.c_str());
	if ( image.format() != QImage::Format_Grayscale8 ) {
		image = image.convertToFormat(QImage::Format_Grayscale8);
	}
	image_u8_t img = {
	                  .width = image.width(),
	                  .height = image.height(),
	                  .stride = image.bytesPerLine(),
	                  .buf =  image.bits(),
	};


	zarray_t * detections = NULL;
	detections = apriltag_detector_detect(d_detector.get(),&img);
	apriltag_detection_t * d;
	tp.Results.reserve(zarray_size(detections));
	for ( size_t i = 0; i < zarray_size(detections); ++i ) {
		zarray_get(detections,i,&d);
		if ( tp.Filter != NULL && d->id != *(tp.Filter) ) {
			continue;
		}
		tp.Results.push_back(Snapshot::FromApriltag(d,
		                                            tp.RelativeImagePath,
		                                            tp.Frame));
	}
}

size_t SnapshotIndexer::start() {
	for ( const auto & de : fs::directory_iterator(d_tdd.FilePath() / "ants" ) ) {
		auto ext = de.path().extension().string();
		std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return std::tolower(c);});
		if ( ext != ".png" ) {
			continue;
		}
		ImageToProcess toProcess;
		toProcess.Basedir = d_basedir;
		toProcess.RelativeImagePath = fs::relative(de.path(),d_tdd.FilePath());
		toProcess.Filter = NULL;

		std::regex filtered("ant_([0-9]+)_frame_([0-9]+).png");
		std::smatch ID;
		std::string filename = de.path().filename().string();
		uint64_t frameNumber;
		if(std::regex_search(filename,ID,filtered) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));

			toProcess.Filter = new fort::myrmidon::priv::TagID(0);
			IDS >> *(toProcess.Filter);
			FrameS >> frameNumber;
			toProcess.Frame = *d_tdd.FrameAt(frameNumber);
			d_toProcess.push_back(toProcess);
			continue;
		}
		filtered =  std::regex("frame_([0-9]+).png");
		if(std::regex_search(filename,ID,filtered) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(1));
			FrameS >> frameNumber;
			toProcess.Frame = *d_tdd.FrameAt(frameNumber);
			d_toProcess.push_back(toProcess);
			continue;
		}
	}
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_quit = false;
	}
	d_future = QtConcurrent::run(QThreadPool::globalInstance(),
	                             [this]() {
		                             LoadCache();
		                             {
			                             std::lock_guard<std::mutex> lock(d_mutex);
			                             if ( d_quit == true ) {
				                             return;
			                             }
		                             }

		                             size_t done = 0;
		                             for(auto & toProcess : d_toProcess ) {
			                             {
				                             std::lock_guard<std::mutex> lock(d_mutex);
				                             if ( d_quit == true ) {
					                             return;
				                             }
			                             }
			                             Process(toProcess);
			                             ++done;
			                             emit resultReady(toProcess.Results,done);
		                             }
		                             SaveCache();
	                             });

	return d_toProcess.size();
}

void SnapshotIndexer::cancel() {
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_quit = true;
	}
	d_future.waitForFinished();
}

Snapshot::ConstPtr SnapshotIndexer::LoadSnapshot(const fort::myrmidon::pb::Snapshot & pb) {
	if (pb.corners_size() != 4 ) {
		throw std::runtime_error("Not enough corner");
	}
	auto res = std::make_shared<Snapshot>(*d_tdd.FrameAt(pb.frame()),pb.tagvalue());
	res->d_position <<
		pb.tagposition().x(),
		pb.tagposition().y(),
		pb.tagangle();
	for( size_t i = 0; i < 4; ++i ) {
		res->d_corners.push_back(Eigen::Vector2d(pb.corners(i).x(),pb.corners(i).y()));
	}
	res->d_relativeImagePath = fs::path(pb.relativeimagepath());
	return res;
}

void SnapshotIndexer::SaveSnapshot(fort::myrmidon::pb::Snapshot & pb, const Snapshot::ConstPtr & s) {
	auto tagPos = pb.mutable_tagposition();
	tagPos->set_x(s->TagPosition().x());
	tagPos->set_y(s->TagPosition().y());
	pb.set_tagangle(s->TagAngle());
	pb.set_tagvalue(s->TagValue());

	for(const auto & c: s->Corners()) {
		auto cPb = pb.add_corners();
		cPb->set_x(c.x());
		cPb->set_y(c.y());
	}
	pb.set_frame(s->Frame()->FrameID());
	pb.set_relativeimagepath(s->d_relativeImagePath.generic_string());
}



void SnapshotIndexer::LoadCache() {
	typedef fort::myrmidon::utils::ProtobufFileReadWriter<fort::myrmidon::pb::SnapshotCacheHeader,
	                                                      fort::myrmidon::pb::Snapshot>
		ReadWriter;
	try {
		ReadWriter::Read(d_tdd.FilePath() / "ants" / "snapshot.cache",
		                 [this](const fort::myrmidon::pb::SnapshotCacheHeader & h) {
			                 if ( h.threshold() != d_detector->qtp.min_white_black_diff ) {
				                 std::ostringstream os;
				                 os << "Threshold cache value (" << h.threshold()
				                    << " is different from expected: " << d_detector->qtp.min_white_black_diff;
				                 throw std::runtime_error(os.str());
			                 }

			                 if ( h.familyname() != std::string(d_family->name) ){
				                 std::ostringstream os;
				                 os << "Family cache value (" << h.familyname()
				                    << " is different from expected: " << d_family->name;
				                 throw std::runtime_error(os.str());
			                 }
		                 },
		                 [this](const fort::myrmidon::pb::Snapshot & s) {
			                 try {
				                 d_cache[s.relativeimagepath()].push_back(LoadSnapshot(s));
			                 } catch ( const std::runtime_error & ) {
			                 }
		                 });
	} catch ( const std::exception & e) {
		std::cerr << "Could not load cache : " << e.what() << std::endl;
	}
}

void SnapshotIndexer::SaveCache() {
	typedef fort::myrmidon::utils::ProtobufFileReadWriter<fort::myrmidon::pb::SnapshotCacheHeader,
	                                                      fort::myrmidon::pb::Snapshot>
		ReadWriter;


	fort::myrmidon::pb::SnapshotCacheHeader h;
	h.set_threshold(d_detector->qtp.min_white_black_diff);
	h.set_familyname(d_family->name);

	std::vector<std::function <void( fort::myrmidon::pb::Snapshot & s ) > >lines;
	for(const auto & img : d_toProcess ) {
		for ( const auto & s : img.Results ) {
			lines.push_back([&s,this](fort::myrmidon::pb::Snapshot & pb) {
				                SaveSnapshot(pb,s);
			                });
		}
	}

	try {
		ReadWriter::Write(d_tdd.FilePath() / "ants" / "snapshot.cache",h,lines);
	} catch ( const std::exception & e) {
		std::cerr << "Could not save cache : " << e.what() << std::endl;
	}

}
