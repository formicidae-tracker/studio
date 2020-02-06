#include "Experiment.hpp"

#include "ExperimentReadWriter.hpp"

#include "Ant.hpp"
#include "Identifier.hpp"
#include "Measurement.hpp"
#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"
#include "AntPoseEstimate.hpp"

#include <myrmidon/utils/Checker.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

Experiment::Experiment(const fs::path & filepath )
	: d_absoluteFilepath(fs::absolute(fs::weakly_canonical(filepath)))
	, d_basedir(d_absoluteFilepath.parent_path())
	, d_identifier(Identifier::Create())
	, d_zoneGroup(std::make_shared<Zone::Group>())
	, d_threshold(40)
	, d_family(fort::tags::Family::Undefined)
	, d_defaultTagSize(1.0) {
	CreateMeasurementType(Measurement::HEAD_TAIL_TYPE,
	                      "head-tail");
}

Experiment::Ptr Experiment::Create(const fs::path & filename) {
	return Experiment::Ptr(new Experiment(filename));
}

Experiment::Ptr Experiment::NewFile(const fs::path & filepath) {
	auto absolutePath = fs::absolute(fs::weakly_canonical(filepath));
	auto base = absolutePath;
	base.remove_filename();
	auto res = Create(absolutePath);

	fs::create_directories(base);
	res->Save(absolutePath);
	return res;
}


Experiment::Ptr Experiment::Open(const fs::path & filepath) {
	return ExperimentReadWriter::Open(filepath);
}

void Experiment::Save(const fs::path & filepath) const {
	auto basedir = fs::weakly_canonical(d_absoluteFilepath).parent_path();
	auto newBasedir = fs::weakly_canonical(filepath).parent_path();
	//TODO: should not be an error.
	if ( basedir != newBasedir ) {
		throw std::runtime_error("Changing experiment file directory is not yet supported");
	}

	ExperimentReadWriter::Save(*this,filepath);
}

Zone::Ptr Experiment::CreateZone(const std::string & name) {
	return Zone::Group::Create(d_zoneGroup,name);
}

void Experiment::DeleteZone(const fs::path & zoneURI) {
	d_zoneGroup->DeleteZone(zoneURI);
}

const std::vector<Zone::Ptr> & Experiment::Zones() const {
	return d_zoneGroup->Zones();
}

const std::map<fs::path,TrackingDataDirectoryConstPtr> &
Experiment::TrackingDataDirectories() const {
	return d_zoneGroup->TrackingDataDirectories();
}

void Experiment::DeleteTrackingDataDirectory(const fs::path & URI) {
	auto fi = std::find_if(d_measurementByURI.begin(),
	                       d_measurementByURI.end(),
	                       [URI](const std::pair<fs::path,MeasurementByType> & elem) -> bool {
		                       if ( elem.first.lexically_relative(URI).empty() == true ) {
			                       return false;
		                       }
		                       return elem.second.empty() == false;
	                       });
	if ( fi != d_measurementByURI.end() ) {
		throw std::runtime_error("Could not remove TrackingDataDirectory '" + URI.generic_string()
		                         + "': it contains measurement '"
		                         + fi->first.generic_string()
		                         + "'");

	}

	d_zoneGroup->DeleteTrackingDataDirectory(URI);

}


const std::string & Experiment::Name() const {
	return d_name;
}

void Experiment::SetName(const std::string & name) {
	d_name = name;
}

const std::string & Experiment::Author() const {
	return d_author;
}

void Experiment::SetAuthor(const std::string & author) {
	d_author = author;
}

const std::string & Experiment::Comment() const {
	return d_comment;
}

void Experiment::SetComment(const std::string & comment) {
	d_comment = comment;
}

uint8_t Experiment::Threshold() const {
	return d_threshold;
}

void Experiment::SetThreshold(uint8_t th) {
	if ( th == 0 ) {
		th = 1;
	}
	if ( th == 255 ) {
		th = 254;
	}
	d_threshold = th;
}

const fs::path & Experiment::AbsoluteFilePath() const {
	return d_absoluteFilepath;
}

const fs::path & Experiment::Basedir() const {
	return d_basedir;
}


fort::tags::Family Experiment::Family() const {
	return d_family;
}

void Experiment::SetFamily(fort::tags::Family tf) {
	d_family = tf;
}


void Experiment::SetMeasurement(const Measurement::ConstPtr & m) {
	if ( d_measurementTypeByID.count(m->Type()) == 0 ) {
		throw std::runtime_error("Unknown MeasurementType::ID " + std::to_string(m->Type()));
	}

	fs::path tddPath;
	FrameID FID;
	TagID TID;
	m->DecomposeURI(tddPath,FID,TID);
	auto fi = d_zoneGroup->TrackingDataDirectories().find(tddPath.generic_string());
	if ( fi == d_zoneGroup->TrackingDataDirectories().end() ) {
		std::ostringstream oss;
		oss << "Unknow data directory " << tddPath;
		throw std::invalid_argument(oss.str());
	}

	auto ref = fi->second->FrameReferenceAt(FID);

	d_measurementByURI[m->TagCloseUpURI()][m->Type()] = m;
	d_measurements[m->Type()][TID][tddPath][ref.Time()] = m;

	if (m->Type() != Measurement::HEAD_TAIL_TYPE) {
		return;
	}

	d_identifier->SetAntPoseEstimate(std::make_shared<AntPoseEstimate>(ref,
	                                                                   TID,
	                                                                   m->EndFromTag(),
	                                                                   m->StartFromTag()));
}

void Experiment::DeleteMeasurement(const fs::path & URI) {
	throw std::runtime_error("Not yet implemented");
}

void Experiment::ListAllMeasurements(std::vector<MeasurementConstPtr> & list) const {
	list.clear();

	size_t s = 0;
	for (const auto & [p,ms] : d_measurementByURI ) {
		for (const auto & [t,m] : ms ) {
			++s;
		}
	}
	list.reserve(s);

	for (const auto & [p,ms] : d_measurementByURI ) {
		for (const auto & [t,m] : ms ) {
			list.push_back(m);
		}
	}
}

double Experiment::DefaultTagSize() const {
	return d_defaultTagSize;
}

void Experiment::SetDefaultTagSize(double defaultTagSize) {
	d_defaultTagSize = defaultTagSize;
}


double Experiment::CornerWidthRatio(tags::Family f) {
	if ( f == tags::Family::Tag36ARTag ) {
		return 1.0;
	}

	static std::map<tags::Family,double> cache;
	auto fi = cache.find(f);
	if ( fi != cache.end() ) {
		return fi->second;
	}
	auto fDef = TagCloseUp::Lister::LoadFamily(f);
	auto res = double(fDef->width_at_border) / double(fDef->total_width);
	cache[f] = res;
	return res;
}

void Experiment::ComputeMeasurementsForAnt(std::vector<ComputedMeasurement> & result,
                                           myrmidon::Ant::ID AID,
                                           MeasurementType::ID type) const {
	auto afi = d_identifier->Ants().find(AID);
	if ( afi == d_identifier->Ants().cend() ) {
		throw Identifier::UnmanagedAnt(AID);
	}

	double cornerWidthRatio = CornerWidthRatio(d_family);

	result.clear();

	for ( const auto & ident : afi->second->Identifications() ) {
		double tagSizeMM = d_defaultTagSize;
		if (ident->UseDefaultTagSize() == false ) {
			tagSizeMM = ident->TagSize();
		}
		tagSizeMM *= cornerWidthRatio;

		auto mm1 = d_measurements.find(type);
		if (mm1 == d_measurements.cend() ) {
			continue;
		}

		auto measurementsByTDD = mm1->second.find(ident->TagValue());
		if ( measurementsByTDD == mm1->second.cend() ) {
			continue;
		}

		for(const auto & measurements : measurementsByTDD->second ) {
			auto start = measurements.second.cbegin();
			if ( ident->Start() ) {
				start = measurements.second.lower_bound(*ident->Start());
			}
			auto end = measurements.second.cend();
			if ( ident->End() ) {
				end = measurements.second.upper_bound(*ident->End());
			}
			for ( ; start != end; ++start ) {
				double distance = (start->second->StartFromTag() - start->second->EndFromTag()).norm();
				distance *= tagSizeMM /start->second->TagSizePx();
				result.push_back({start->first,distance});
			}
		}
	}
}

MeasurementType::ID Experiment::NextAvailableMeasurementTypeID() const {
	MeasurementType::ID newType = Measurement::HEAD_TAIL_TYPE + 1;
	// eager approach, but I won't see user makes 10 000 different
	// measurement type for each 1000 ants in a colony.
	while( d_measurementTypeByID.count(newType) != 0 ) {
		++newType;
	}
	return newType;
}


MeasurementType::Ptr Experiment::CreateMeasurementType(MeasurementType::ID MTID,
                                                       const std::string & name) {
	if ( d_measurementTypeByID.count(MTID) != 0 ) {
		throw std::runtime_error("MeasurementType::ID "
		                         + std::to_string(MTID)
		                         + " is already used");
	}
	auto res = std::make_shared<MeasurementType>(MTID,name);
	d_measurementTypeByID.insert(std::make_pair(MTID,res));
	return res;
}

void Experiment::DeletedMeasurementType(MeasurementType::ID MTID) {
	auto fi = d_measurementTypeByID.find(MTID);
	if ( fi == d_measurementTypeByID.end()  ) {
		throw std::runtime_error("Could not find MeasurementType::ID " + std::to_string(MTID));
	}

	if ( d_measurements.count(MTID) != 0 ) {
		throw std::runtime_error("Could not remove MeasurementType '" + fi->second->Name() + "' has experiment still contains measurement");
	}

	if (MTID == Measurement::HEAD_TAIL_TYPE ) {
		throw std::invalid_argument("Could not remove default measurement type 'head-tail'");
	}

	d_measurementTypeByID.erase(fi);
}

const Experiment::MeasurementTypeByID & Experiment::MeasurementTypes() const {
	return d_measurementTypeByID;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
