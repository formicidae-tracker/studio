#include "Measurement.hpp"


#include "TagCloseUp.hpp"


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

const MeasurementType::ID Measurement::HEAD_TAIL_TYPE = 1;

Measurement::Measurement(const std::string & parentURI,
                         MeasurementType::ID mtID,
                         const Eigen::Vector2d & startFromTag,
                         const Eigen::Vector2d & endFromTag,
                         double tagSizePx)
	: d_start(startFromTag)
	, d_end(endFromTag)
	, d_mtID(mtID)
	, d_tagSizePx(tagSizePx) {
	// We ensure a correctly formatted URI
	auto [tddURI,frameID,tagID,mtIDIgnored] = DecomposeURI( (fs::path(parentURI) / "measurements" / std::to_string(mtID)).generic_string());
	d_URI = (fs::path(TagCloseUp::FormatURI(tddURI,frameID,tagID)) / "measurements" / std::to_string(d_mtID)).generic_string();

}

Measurement::~Measurement() {}

const std::string & Measurement::URI() const{
	return d_URI;
}

std::string Measurement::TagCloseUpURI() const {
	return fs::path(d_URI).parent_path().parent_path().generic_string();
}


MeasurementType::ID Measurement::Type() const {
	return d_mtID;
}

const Eigen::Vector2d & Measurement::StartFromTag() const {
	return d_start;
}

const Eigen::Vector2d & Measurement::EndFromTag() const {
	return d_end;
}

std::tuple<std::string,FrameID,TagID,MeasurementType::ID>
Measurement::DecomposeURI(const std::string & measurementURI) {
	std::string tddURI;
	FrameID frameID;
	TagID  tagID;
	MeasurementType::ID mtID;
	fs::path URI = measurementURI;

	try {
		try {
			mtID = std::stoul(URI.filename().string());
		} catch( const std::exception & e) {
			throw std::runtime_error("cannot parse MeasurementType::ID");
		}
		URI = URI.parent_path();
		if ( URI.filename() != "measurements" ) {
			throw std::runtime_error("no 'measurements' in URI");
		}
		URI = URI.parent_path();
		try {
			tagID = std::stoul(URI.filename().string(),NULL,0);
		} catch( const std::exception & e) {
			throw std::runtime_error("cannot parse TagID");
		}
		URI = URI.parent_path();
		if ( URI.filename() != "closeups" ) {
			throw std::runtime_error("no 'closeups' in URI");
		}
		URI = URI.parent_path();
		try {
			frameID = std::stoull(URI.filename().string());
		} catch( const std::exception & e) {
			throw std::runtime_error("cannot parse FrameID");
		}
		URI = URI.parent_path();
		if ( URI.filename() != "frames" ) {
			throw std::runtime_error("no 'frames' in URI");
		}
		tddURI = URI.parent_path().generic_string();
		if (tddURI.empty() || tddURI == "/" ) {
			throw std::runtime_error("no URI for TrackingDataDirectory");
		}
	} catch ( const std::exception & e) {
		throw std::runtime_error("Invalid URI '"
		                         + measurementURI
		                         + "':" + e.what());
	}
	return std::make_tuple(tddURI,frameID,tagID,mtID);
}

double Measurement::TagSizePx() const {
	return d_tagSizePx;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
