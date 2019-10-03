#include "FramePointer.hpp"



using namespace fort::myrmidon::priv;

void FramePointer::Encode(fort::myrmidon::pb::FramePointer & fp) const{
	fp.Clear();
	fp.set_path(Path.generic_string());
	fp.set_frame(Frame);
	fp.mutable_pathstartdate()->CheckTypeAndMergeFrom(PathStartDate);
}

FramePointer::Ptr FramePointer::FromSaved(const fort::myrmidon::pb::FramePointer & fp) {
	if ( fp.path().empty() == true ) {
		return FramePointer::Ptr();
	}
	auto res = std::make_shared<FramePointer>();
	res->Path = std::filesystem::path(fp.path(),std::filesystem::path::generic_format);
	res->Frame = fp.frame();
	res->PathStartDate.CheckTypeAndMergeFrom(fp.pathstartdate());
	return res;
}
