#include "TagCloseUp.hpp"

#include <regex>

#include <fort/myrmidon/utils/Checker.hpp>
#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>
#include <fort/myrmidon/priv/proto/IOUtils.hpp>
#include <fort/myrmidon/TagCloseUpCache.pb.h>

#include <fort/myrmidon/utils/Defer.hpp>

#include <opencv2/imgcodecs.hpp>

#include <iostream>

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


std::string TagCloseUp::FormatURI(const std::string & tddURI,
                                  FrameID frameID,
                                  TagID tagID) {
	return (fs::path(tddURI) / "frames" / std::to_string(frameID) /  "closeups" / FormatTagID(tagID)).generic_string();
}


TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       TagID tagID,
                       const Eigen::Vector2d & position,
                       double angle,
                       const Vector2dList & corners)
	: d_reference(reference)
	, d_URI(FormatURI(reference.ParentURI(),reference.FrameID(),tagID))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(tagID)
	, d_tagPosition(position)
	, d_tagAngle(angle)
	, d_corners(corners) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
	if (corners.size() != 4 ) {
		throw std::invalid_argument("A tag needs 4 corners, only got " + std::to_string(corners.size()));
	}
}

TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       const apriltag_detection_t * d)
	: d_reference(reference)
	, d_URI( FormatURI(reference.ParentURI(),reference.FrameID(),d->id) )
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

const std::string & TagCloseUp::URI() const {
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

const Vector2dList & TagCloseUp::Corners() const {
	return d_corners;
}

TagCloseUp::Lister::Ptr
TagCloseUp::Lister::Create(const fs::path & absoluteBaseDir,
                           const tags::ApriltagOptions & detectorOptions,
                           FrameReferenceResolver resolver,
                           bool forceCache) {
	Ptr res(new Lister(absoluteBaseDir,
	                   detectorOptions,
	                   resolver,
	                   forceCache));
	res->d_itself = res;
	return res;
}



TagCloseUp::Lister::Lister(const fs::path & absoluteBaseDir,
                           const tags::ApriltagOptions & detectorOptions,
                           FrameReferenceResolver resolver,
                           bool forceCache)
	: d_absoluteBaseDir(absoluteBaseDir)
	, d_detectorOptions(detectorOptions)
	, d_resolver(resolver) {
	PERF_FUNCTION();
	d_saveCacheOnDelete = true;
	try {
		LoadCache();
		d_saveCacheOnDelete = false;
	} catch (const std::exception & e) {
		if ( forceCache == true ) {
			throw std::runtime_error(std::string("Could not list from cache: ") + e.what());
		}
	}
}

TagCloseUp::Lister::~Lister() {
	if ( d_saveCacheOnDelete == true ) {
		UnsafeSaveCache();
	}
}

std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>>
TagCloseUp::Lister::ListFiles(const fs::path & path) {
	PERF_FUNCTION();
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
		FrameID frameID;
		if(std::regex_search(filename,ID,singleRx) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));
			auto tagID = std::make_shared<TagID>(0);

			IDS >> *(tagID);
			FrameS >> frameID;
			res.insert(std::make_pair(frameID,std::make_pair(de.path(),tagID)));
			continue;
		}
		if(std::regex_search(filename,ID,multiRx) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(1));
			FrameS >> frameID;
			res.insert(std::make_pair(frameID,std::make_pair(de.path(),std::shared_ptr<TagID>())));
			continue;
		}

	}

	return res;
}



fs::path TagCloseUp::Lister::CacheFilePath(const fs::path & filepath) {
	return filepath / "tag-close-up.cache";
}

std::pair<apriltag_family_t*,TagCloseUp::Lister::ATFamilyDestructor>
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
	return std::make_pair(fi->second.c(),fi->second.d);
}

void TagCloseUp::Lister::UnsafeSaveCache() {
	typedef proto::FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> RW;

	auto cachePath = CacheFilePath(d_absoluteBaseDir);

	pb::TagCloseUpCacheHeader h;
	h.set_threshold(d_detectorOptions.QuadMinBWDiff);
	h.set_family(proto::IOUtils::SaveFamily(d_detectorOptions.Family));
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
	PERF_FUNCTION();
	typedef proto::FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> RW;

	auto cachePath = CacheFilePath(d_absoluteBaseDir);

	RW::Read(cachePath,
	         [this](const pb::TagCloseUpCacheHeader & pb) {
		         if ( proto::IOUtils::LoadFamily(pb.family()) != d_detectorOptions.Family ) {
			         throw std::runtime_error("Mismatched cached tag family");
		         }
		         if ( pb.threshold() != d_detectorOptions.QuadMinBWDiff ) {
			         throw std::runtime_error("Mismatched cache threshold");
		         }
	         },
	         [this]( const pb::TagCloseUp & line) {
		         auto tcu = proto::IOUtils::LoadTagCloseUp(line,
		                                                   d_absoluteBaseDir,
		                                                   d_resolver);
		         auto relativePath = fs::relative(tcu->AbsoluteFilePath(),d_absoluteBaseDir);
		         d_cache[relativePath].push_back(tcu);
	         });
}


apriltag_detector_t *
TagCloseUp::Lister::CreateDetector() {
	apriltag_detector_t * detector =  apriltag_detector_create();
	d_detectorOptions.SetUpDetector(detector);
	return detector;
}


TagCloseUp::List TagCloseUp::Lister::LoadFileFromCache(const fs::path & file) {
	return d_cache.at(file);
}

TagCloseUp::List TagCloseUp::Lister::LoadFile(const FileAndFilter & f,
                                              FrameID frameID,
                                              size_t nbFiles) {
	auto relativePath = fs::relative(f.first,d_absoluteBaseDir);

	auto ref = d_resolver(frameID);

	if ( d_detectorOptions.Family == tags::Family::Undefined ) {
		return {};
	}

	std::vector<ConstPtr> tags;
	apriltag_detector_t * detector = CreateDetector();

	Defer saveToCache([&,this]() {
		                  std::lock_guard<std::mutex> lock(d_mutex);
		                  d_cache.insert(std::make_pair(relativePath,tags));
	                  });

	auto [family,family_destructor] = LoadFamily(d_detectorOptions.Family);
	apriltag_detector_add_family(detector,family);
	Defer destroyDetector([detector,
	                       family = family,
	                       family_destructor = family_destructor ]() {
		                      apriltag_detector_destroy(detector);
		                      family_destructor(family);
	                      });

	auto imgCv = cv::imread(f.first.string(),cv::IMREAD_GRAYSCALE);

	if ( imgCv.empty() ) {
		return tags;
	}

	image_u8_t img =
		{
		 .width = imgCv.cols,
		 .height = imgCv.rows,
		 .stride = imgCv.cols,
		 .buf = imgCv.data
		};
	zarray_t * detections
		= apriltag_detector_detect(detector,&img);
	Defer destroyDetections([detections]() {
							  apriltag_detections_destroy(detections);
							});

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


	return tags;
}



std::vector<TagCloseUp::Lister::Loader> TagCloseUp::Lister::PrepareLoaders() {
	PERF_FUNCTION();
	auto itself = d_itself.lock();
	if (!itself) {
		throw DeletedReference<Lister>();
	}

	std::vector<Loader> res;

	if ( d_saveCacheOnDelete == false ) {
		res.reserve(d_cache.size());
		for( const auto & [path,list] : d_cache ) {
			res.push_back([=,
			               path = path]() {
				              return itself->LoadFileFromCache(path);
			              });
		}

		return res;
	}

	auto files = ListFiles(d_absoluteBaseDir);
	auto nbFiles = files.size();
	res.reserve(files.size());

	for( const auto & [frameID,f] : files ) {
		res.push_back([=,
		               f = f,
		               frameID = frameID]() {
			              return itself->LoadFile(f,frameID,nbFiles);
		              });
	}

	return res;
}

const tags::ApriltagOptions & TagCloseUp::Lister::DetectorOptions() const {
	return d_detectorOptions;
}


Isometry2Dd TagCloseUp::ImageToTag() const {
	return Isometry2Dd(d_tagAngle,d_tagPosition).inverse();
}

double TagCloseUp::TagSizePx() const {
	double res = (d_corners[0] - d_corners[1]).norm()
		+ (d_corners[1] - d_corners[2]).norm()
		+ (d_corners[2] - d_corners[3]).norm()
		+ (d_corners[3] - d_corners[0]).norm();

	return res / 4.0;
}

double TagCloseUp::Squareness() const {
	double maxAngleDistanceToPI_2 = 0.0;
	for(size_t i = 0 ; i < 4; ++i ) {
		Eigen::Vector2d a = d_corners[(i-1)%4] - d_corners[i];
		Eigen::Vector2d b = d_corners[(i+1)%4] - d_corners[i];
		double aNorm = a.norm();
		double bNorm = b.norm();
		if ( aNorm < 1.0e-3 || bNorm < 1.0e-3 ) {
			return 0;
		}
		double angle = std::acos(a.dot(b)/ (aNorm * bNorm));
		maxAngleDistanceToPI_2 = std::max(maxAngleDistanceToPI_2,
		                                  std::abs(angle - (M_PI / 2.0)));
	}
	return 1.0 - maxAngleDistanceToPI_2 / M_PI * 2.0;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & p) {
	return out << p.Frame() << "/closeups/"
	           << fort::myrmidon::FormatTagID(p.TagValue());
}
