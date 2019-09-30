#include "FramePointer.hpp"



using namespace fort::myrmidon::priv;

void FramePointer::Encode(fort::myrmidon::pb::FramePointer & fp){
	fp.Clear();
	fp.set_path(Path.string());
	fp.set_frame(Frame);
	fp.mutable_pathstartdate()->CheckTypeAndMergeFrom(PathStartDate);
}

FramePointer::Ptr FramePointer::FromSaved(const fort::myrmidon::pb::FramePointer & fp) {
	if ( fp.path().empty() == true ) {
		return FramePointer::Ptr();
	}
	auto res = std::make_shared<FramePointer>();
	res->Path = fp.path();
	res->Frame = fp.frame();
	res->PathStartDate.CheckTypeAndMergeFrom(fp.pathstartdate());
	return res;
}
