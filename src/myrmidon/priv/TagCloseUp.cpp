#include "TagCloseUp.hpp"

#include <regex>

#include <myrmidon/utils/Checker.hpp>
#include <myrmidon/priv/proto/FileReadWriter.hpp>
#include <myrmidon/priv/proto/IOUtils.hpp>
#include <myrmidon/TagCloseUpCache.pb.h>

#include <myrmidon/utils/Defer.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


double TagCloseUp::ComputeAngleFromCorners(const Eigen::Vector2d & c0,
                                           const Eigen::Vector2d & c1,
                                           const Eigen::Vector2d & c2,
                                           const Eigen::Vector2d & c3) {
	Eigen::Vector2d delta = (c1 + c2) / 2.0 - (c0 + c3) / 2.0;
	return atan2(delta.y(),delta.x());
}


TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       TagID tid,
                       const Eigen::Vector2d & position,
                       double angle,
                       const Vector2dList & corners)
	: d_reference(reference)
	, d_URI(d_reference.URI() / "closeups" / std::to_string(tid))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(tid)
	, d_tagPosition(position)
	, d_tagAngle(angle)
	, d_corners(corners) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
}

TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       const apriltag_detection_t * d)
	: d_reference(reference)
	, d_URI(d_reference.URI() / "closeups" / std::to_string(d->id))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(d->id)
	, d_tagPosition(d->c[0],d->c[1])
	, d_tagAngle(0.0)
	, d_corners(4) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
	for(size_t i = 0; i < 4; ++i ) {
		d_corners[i] << d->p[i][0],d->p[i][1];
	}
	d_tagAngle = ComputeAngleFromCorners(d_corners[0],
	                                     d_corners[1],
	                                     d_corners[2],
	                                     d_corners[3]);

}


TagCloseUp::~TagCloseUp() {}

const FrameReference & TagCloseUp::Frame() const {
	return d_reference;
}

const fs::path & TagCloseUp::URI() const {
	return d_URI;
}

const fs::path & TagCloseUp::AbsoluteFilePath() const  {
	return d_absoluteFilePath;
}

TagID TagCloseUp::TagValue() const {
	return d_tagID;
}

const Eigen::Vector2d & TagCloseUp::TagPosition() const {
	return d_tagPosition;
}

double TagCloseUp::TagAngle() const {
	return d_tagAngle;
}

const TagCloseUp::Vector2dList & TagCloseUp::Corners() const {
	return d_corners;
}

TagCloseUp::Lister::Lister(const fs::path & absoluteBaseDir,
                           tags::Family f,
                           uint8_t threshold,
                           FrameReferenceResolver resolver)
	: d_absoluteBaseDir(absoluteBaseDir)
	, d_family(f)
	, d_threshold(threshold)
	, d_resolver(resolver)
	, d_atfamily(LoadFamily(f)){
	try {
		LoadCache();
	} catch (const std::exception & e) {
		//LOG(INFO) << "Cannot load cache for " << absoluteBaseDir << ": " << e.what();
	}
}

std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>>
TagCloseUp::Lister::ListFiles(const fs::path & path) {
	std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> res;

	static std::regex singleRx("ant_([0-9]+)_frame_([0-9]+).png");
	static std::regex multiRx("frame_([0-9]+).png");

	for ( const auto & de : fs::directory_iterator(path) ) {
		auto ext = de.path().extension().string();
		std::transform(ext.begin(),ext.end(),ext.begin(),
		               [](unsigned char c) {
			               return std::tolower(c);
		               });
		if ( ext != ".png" ) {
			continue;
		}

		std::smatch ID;
		std::string filename = de.path().filename().string();
		FrameID FID;
		if(std::regex_search(filename,ID,singleRx) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));
			auto TID = std::make_shared<TagID>(0);

			IDS >> *(TID);
			FrameS >> FID;
			res.insert(std::make_pair(FID,std::make_pair(de.path(),TID)));
			continue;
		}
		if(std::regex_search(filename,ID,multiRx) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(1));
			FrameS >> FID;
			res.insert(std::make_pair(FID,std::make_pair(de.path(),std::shared_ptr<TagID>())));
			continue;
		}

	}

	return res;
}



fs::path TagCloseUp::Lister::CacheFilePath(const fs::path & filepath) {
	return filepath / "tag-close-up.cache";
}

std::shared_ptr<apriltag_family_t>
TagCloseUp::Lister::LoadFamily(tags::Family family) {
	struct FamilyInterface {
		typedef apriltag_family_t* (*Constructor) ();
		typedef void (*Destructor) (apriltag_family_t *);
		Constructor c;
		Destructor  d;
	};
	static std::map<tags::Family,FamilyInterface>  familyFactory = {
		 {fort::tags::Family::Tag16h5,{.c = tag16h5_create, .d = tag16h5_destroy}},
		 {fort::tags::Family::Tag25h9,{.c =tag25h9_create, .d=tag25h9_destroy}},
		 {fort::tags::Family::Tag36h10,{.c =tag36h10_create, .d=tag36h10_destroy}},
		 {fort::tags::Family::Tag36h11,{.c =tag36h11_create, .d=tag36h11_destroy}},
		 {fort::tags::Family::Tag36ARTag,{.c =tag36ARTag_create, .d=tag36ARTag_destroy}},
		 {fort::tags::Family::Circle21h7,{.c =tagCircle21h7_create, .d=tagCircle21h7_destroy}},
		 {fort::tags::Family::Circle49h12,{.c =tagCircle49h12_create, .d=tagCircle49h12_destroy}},
		 {fort::tags::Family::Custom48h12,{.c =tagCustom48h12_create, .d=tagCustom48h12_destroy}},
		 {fort::tags::Family::Standard41h12,{.c =tagStandard41h12_create, .d=tagStandard41h12_destroy}},
		 {fort::tags::Family::Standard52h13,{.c =tagStandard52h13_create, .d=tagStandard52h13_destroy}},
	};

	auto fi = familyFactory.find(family);
	if ( fi == familyFactory.cend() ) {
		std::ostringstream oss;
		oss << "Unsupported family: " << (int)family;
		throw std::invalid_argument(oss.str());
	}
	return std::shared_ptr<apriltag_family_t>(fi->second.c(),fi->second.d);
}

void TagCloseUp::Lister::UnsafeSaveCache() {
	typedef proto::FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> RW;

	auto cachePath = CacheFilePath(d_absoluteBaseDir);
	pb::TagCloseUpCacheHeader h;
	h.set_threshold(d_threshold);
	h.set_family(proto::IOUtils::SaveFamily(d_family));
	std::vector<RW::LineWriter> lines;
	for ( const auto & [p,tcus] : d_cache ) {
		for (const auto & tcu : tcus ) {
			lines.push_back([tcu,this](pb::TagCloseUp & pb) {
				                proto::IOUtils::SaveTagCloseUp(&pb,
				                                               tcu,
				                                               d_absoluteBaseDir);
			                });
		}
	}
	RW::Write(cachePath,h,lines);
}

void TagCloseUp::Lister::LoadCache() {
	typedef proto::FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> RW;

	auto cachePath = CacheFilePath(d_absoluteBaseDir);

	RW::Read(cachePath,
	         [this](const pb::TagCloseUpCacheHeader & pb) {
		         if ( proto::IOUtils::LoadFamily(pb.family()) != d_family ) {
			         throw std::runtime_error("Mismatched cached tag family");
		         }
		         if ( pb.threshold() != d_threshold ) {
			         throw std::runtime_error("Mismatched cache threshold");
		         }
	         },
	         [this]( const pb::TagCloseUp & line) {
		         auto tcu = proto::IOUtils::LoadTagCloseUp(line,
		                                                   d_absoluteBaseDir,
		                                                   d_resolver);
		         auto relativePath = fs::relative(tcu->AbsoluteFilePath(),d_absoluteBaseDir);
		         std::lock_guard<std::mutex> lock(d_mutex);
		         auto fi = d_cache.find(relativePath);
		         if ( fi != d_cache.end() ) {
			         fi->second.push_back(tcu);
		         } else {
			         d_cache[relativePath] = {tcu};
		         }
	         });

	d_cacheModified = false;
}


std::shared_ptr<apriltag_detector_t>
TagCloseUp::Lister::CreateDetector() {
	std::shared_ptr<apriltag_detector_t>
		detector(apriltag_detector_create(),
		         apriltag_detector_destroy);

	apriltag_detector_add_family(detector.get(),d_atfamily.get());
	detector->qtp.min_white_black_diff = d_threshold;

	detector->nthreads =1;
	detector->nthreads = 1;
	detector->quad_decimate = 1.0;
	detector->quad_sigma = 0.0;
	detector->refine_edges = 0;
	detector->debug = false;
	detector->qtp.min_cluster_pixels = 25;
	detector->qtp.max_nmaxima = 10;
	detector->qtp.critical_rad = 10.0 * M_PI / 180.0;
	detector->qtp.max_line_fit_mse = 10.0;
	detector->qtp.deglitch = 0;
	return detector;
}


std::vector<TagCloseUp::Lister::Loader> TagCloseUp::Lister::PrepareLoaders() {
	auto files = ListFiles(d_absoluteBaseDir);
	auto nbFiles = files.size();
	std::vector<Loader> res;
	res.reserve(files.size());

	for( const auto & [FID,f] : files ) {
		res.push_back([this,f,FID,nbFiles]() {
			              Defer cleanup([this,nbFiles]() {
				                            std::lock_guard<std::mutex> lock(d_mutex);
				                            ++d_parsed;
				                            if ( d_cacheModified && d_parsed == nbFiles ) {
					                            UnsafeSaveCache();
					                            d_cacheModified = false;
				                            }
			                            });

			              auto relativePath = fs::relative(f.first,d_absoluteBaseDir);
			              {
				              std::lock_guard<std::mutex> lock(d_mutex);
				              auto fi = d_cache.find(relativePath);
				              if ( fi != d_cache.cend() ) {
					              return fi->second;
				              }
			              }

			              auto ref = d_resolver(FID);

			              std::vector<ConstPtr> tags;
			              auto detector = CreateDetector();

			              //TODO open image

			              image_u8_t img {

			              };
			              zarray_t * detections
				              = apriltag_detector_detect(detector.get(),&img);

			              apriltag_detection * d;

			              for(size_t i = 0; i < zarray_size(detections); ++i ) {
				              zarray_get(detections,i,&d);
				              if (f.second && d->id != *(f.second)) {
					              continue;
				              }
				              tags.push_back(std::make_shared<TagCloseUp>(f.first,
				                                                          ref,
				                                                          d));
			              }

			              apriltag_detections_destroy(detections);
			              {
				              std::lock_guard<std::mutex> lock(d_mutex);
				              d_cache.insert(std::make_pair(relativePath,tags));
				              d_cacheModified = true;
			              }

			              return tags;

		              });

	}
	return res;

}


} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & p) {
	return out << p.Frame() << "/closeups/" << p.TagValue();
}
