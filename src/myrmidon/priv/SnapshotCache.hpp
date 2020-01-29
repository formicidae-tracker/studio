#pragma once

#include <map>

#include <myrmidon/utils/FileSystem.hpp>

#include "Types.hpp"

namespace fort {

namespace myrmidon {

namespace priv {




class SnapshotIndex {
public:
	static std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> ListSnapshotFiles(const fs::path & path);
private:


};

} // namespace priv

} // namespace myrmidon

} // namespace forty
