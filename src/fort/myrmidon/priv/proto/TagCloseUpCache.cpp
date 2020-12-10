#include "TagCloseUpCache.hpp"


#include "IOUtils.hpp"


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


const uint32_t TagCloseUpCache::CACHE_VERSION = 1;

const std::string TagCloseUpCache::CACHE_PATH = "ants/tag-close-up.cache";

std::vector<TagCloseUp::ConstPtr>
TagCloseUpCache::Load(const fs::path & tddAbsoluteFilePath,
                         std::function<FrameReference (FrameID)> resolver) {
	std::vector<TagCloseUp::ConstPtr> res;
	ReadWriter::Read(tddAbsoluteFilePath / CACHE_PATH ,
	                 [&res](const pb::TagCloseUpCacheHeader & pb) {
		                 if ( pb.version() != CACHE_VERSION) {
			                 throw std::runtime_error("Mismatched cache version "
			                                          + std::to_string(pb.version())
			                                          + " (expected:"
			                                          + std::to_string(CACHE_VERSION));
		                 }
	                 },
	                 [&] ( const pb::TagCloseUp & line) {
		                 auto tcu = proto::IOUtils::LoadTagCloseUp(line,
		                                                           tddAbsoluteFilePath / "ants",
		                                                           resolver);
		                 res.push_back(tcu);
	                 });
	return res;
}

void TagCloseUpCache::Save(const fs::path & tddAbsoluteFilePath,
                           const std::vector<TagCloseUp::ConstPtr> & tagCloseUps) {
	pb::TagCloseUpCacheHeader h;
	h.set_version(CACHE_VERSION);
	std::vector<ReadWriter::LineWriter> lines;

	for ( const auto & tcu : tagCloseUps ) {
		lines.push_back([tcu = std::ref(tcu),
		                 &tddAbsoluteFilePath](pb::TagCloseUp & line) {
			                proto::IOUtils::SaveTagCloseUp(&line,tcu,tddAbsoluteFilePath / "ants");
		                });
	}

	ReadWriter::Write(tddAbsoluteFilePath / CACHE_PATH,
	                  h,
	                  lines);
}

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
