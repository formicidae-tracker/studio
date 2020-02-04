#include "Measurement.hpp"



namespace fort {
namespace myrmidon {
namespace priv {

Measurement::Type::Type(ID TID,const std::string & name)
	: d_TID(TID)
	, d_name(name) {
}


const std::string & Measurement::Type::Name() const {
	return d_name;
}

Measurement::Type::ID Measurement::Type::TID() const {
	return d_TID;
}

Measurement::Measurement(const fs::path & parentURI,
                         Type::ID TID,
                         const Eigen::Vector2d & startFromTag,
                         const Eigen::Vector2d & endFromTag)
	: d_start(startFromTag)
	, d_end(endFromTag)
	, d_TID(TID)
	, d_URI( parentURI / "measurements" / std::to_string(TID) ) {
}

const fs::path & Measurement::URI() const{
	return d_URI;
}

fs::path Measurement::TagCloseUpURI() const {
	return d_URI.parent_path().parent_path();
}


Measurement::Type::ID Measurement::Type() const {
	return d_TID;
}

const Eigen::Vector2d & Measurement::StartFromTag() const {
	return d_start;
}

const Eigen::Vector2d & Measurement::EndFromTag() const {
	return d_end;
}

void Measurement::DecomposeURI(fs::path & tddURI,
                               FrameID & FID,
                               TagID & TID) const {
	TID = std::stoul(d_URI.filename().string());
	fs::path uri = d_URI;
	uri.remove_filename().remove_filename();
	FID = std::stoull(uri.filename().string());
	tddURI = uri.remove_filename().remove_filename();
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
