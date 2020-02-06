#include "Measurement.hpp"



namespace fort {
namespace myrmidon {
namespace priv {

MeasurementType::MeasurementType(ID TID,const std::string & name)
	: d_TID(TID)
	, d_name(name) {
}


const std::string & MeasurementType::Name() const {
	return d_name;
}

void MeasurementType::SetName(const std::string & name) {
	d_name = name;
}

MeasurementType::ID MeasurementType::MTID() const {
	return d_TID;
}

const MeasurementType::ID Measurement::HEAD_TAIL_TYPE = 0;

Measurement::Measurement(const fs::path & parentURI,
                         MeasurementType::ID TID,
                         const Eigen::Vector2d & startFromTag,
                         const Eigen::Vector2d & endFromTag,
                         double tagSizePx)
	: d_start(startFromTag)
	, d_end(endFromTag)
	, d_TID(TID)
	, d_URI( parentURI / "measurements" / std::to_string(TID) )
	, d_tagSizePx(tagSizePx) {
}

const fs::path & Measurement::URI() const{
	return d_URI;
}

fs::path Measurement::TagCloseUpURI() const {
	return d_URI.parent_path().parent_path();
}


MeasurementType::ID Measurement::Type() const {
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
	fs::path parentURI = TagCloseUpURI();
	TID = std::stoul(parentURI.filename().string());
	parentURI.remove_filename().remove_filename();
	FID = std::stoull(parentURI.filename().string());
	tddURI = parentURI.remove_filename().remove_filename();
}

double Measurement::TagSizePx() const {
	return d_tagSizePx;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
