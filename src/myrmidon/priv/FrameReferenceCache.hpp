#pragma once

#include <memory>


namespace fort {
namespace myrmidon {
namespace priv {

class FrameReferenceCache {
public :
	typedef std::shared_ptr<FrameReferenceCache> Ptr;


	static Ptr Open(const fs::path & path, const fs::path & experimentRoot);

	virtual ~FrameReferenceCache();




private:
	TrackingIndexer



};

} // namespace priv
} // namespace myrmidon
} // namespace fort
