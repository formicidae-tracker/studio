#include "Experiment.hpp"

#include "ExperimentReadWriter.hpp"

#include "Ant.hpp"
#include "Identifier.hpp"
#include "Measurement.hpp"
#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"
#include "AntPoseEstimate.hpp"
#include "AntShapeType.hpp"
#include "AntMetadata.hpp"
#include "Capsule.hpp"
#include "InteractionSolver.hpp"

#include <fort/myrmidon/utils/Checker.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

Experiment::Experiment(const fs::path & filepath )
	: d_absoluteFilepath(fs::absolute(fs::weakly_canonical(filepath)))
	, d_basedir(d_absoluteFilepath.parent_path())
	, d_identifier(std::make_shared<fort::myrmidon::priv::Identifier>())
	, d_universe(std::make_shared<Space::Universe>())
	, d_threshold(40)
	, d_family(fort::tags::Family::Undefined)
	, d_defaultTagSize(1.0)
	, d_antShapeTypes(std::make_shared<AntShapeTypeContainer>()) {
	CreateMeasurementType("head-tail",Measurement::HEAD_TAIL_TYPE);

	auto onNameChange =
		[this](const std::string & oldName, const std::string & newName) {
			for ( const auto & [aID,a] : d_identifier->Ants() ) {
				if ( a->DataMap().count(oldName) == 0 ) {
					continue;
				}
				AntDataMap map = a->DataMap();
				map.insert(std::make_pair(newName,map.at(oldName)));
				map.erase(oldName);
				a->SetValues(map);
			}
		};

	auto onTypeChange =
		[this](const std::string & name,
		       AntMetadata::Type oldType,
		       AntMetadata::Type newType) {
			if ( oldType == newType ) {
				return;
			}
			for ( const auto & [aID,a] : d_identifier->Ants() ) {
				if ( a->DataMap().count(name) == 1 ) {
					throw std::runtime_error("Could not change type for column '" + name + "': ant " + Ant::FormatID(a->ID()) + " already contains data");
				}
			}
		};

	auto onDefaultChange =
		[this](const std::string & name,
		       const AntStaticValue &,
		       const AntStaticValue &) {
			for ( const auto & [aID,ant] : d_identifier->Ants() ) {
				ant->CompileData();
			}
		};


	d_antMetadata = std::make_shared<AntMetadata>(onNameChange,
	                                              onTypeChange,
	                                              onDefaultChange);
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

Space::Ptr Experiment::CreateSpace(const std::string & name,Space::ID ID) {
	return Space::Universe::Create(d_universe,ID,name);
}

void Experiment::DeleteSpace(Space::ID ID) {
	d_universe->DeleteSpace(ID);
}

const SpaceByID & Experiment::Spaces() const {
	return d_universe->Spaces();
}

const Space::Universe::TrackingDataDirectoryByURI &
Experiment::TrackingDataDirectories() const {
	return d_universe->TrackingDataDirectories();
}

void Experiment::CheckTDDIsDeletable(const std::string & URI) const {
	auto fi = std::find_if(d_measurementByURI.begin(),
	                       d_measurementByURI.end(),
	                       [URI](const std::pair<fs::path,MeasurementByType> & elem) -> bool {
		                       if ( fs::path(elem.first).lexically_relative(URI).empty() == true ) {
			                       return false;
		                       }
		                       return elem.second.empty() == false;
	                       });
	if ( fi != d_measurementByURI.end() ) {
		throw std::runtime_error("Could not remove TrackingDataDirectory '" + URI
		                         + "': it contains measurement '"
		                         + fi->first
		                         + "'");

	}
}

bool Experiment::TrackingDataDirectoryIsDeletable(const std::string & URI) const {
	try {
		CheckTDDIsDeletable(URI);
	} catch ( const std::exception & e) {
		return false;
	}
	return true;
}


void Experiment::DeleteTrackingDataDirectory(const std::string & URI) {
	CheckTDDIsDeletable(URI);
	d_universe->DeleteTrackingDataDirectory(URI);
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

Ant::Ptr Experiment::CreateAnt(fort::myrmidon::Ant::ID aID) {
	return d_identifier->CreateAnt(d_antShapeTypes,
	                               d_antMetadata,
	                               aID);
}

void Experiment::SetMeasurement(const Measurement::ConstPtr & m) {
	if ( d_measurementTypes.Objects().count(m->Type()) == 0 ) {
		throw std::runtime_error("Unknown MeasurementType::ID " + std::to_string(m->Type()));
	}

	std::string tddURI;
	FrameID FID;
	TagID TID;
	MeasurementType::ID MTID;
	Measurement::DecomposeURI(m->URI(),tddURI,FID,TID,MTID);
	auto fi = d_universe->TrackingDataDirectories().find(tddURI);
	if ( fi == d_universe->TrackingDataDirectories().end() ) {
		std::ostringstream oss;
		oss << "Unknown data directory '" << tddURI << "'";
		throw std::invalid_argument(oss.str());
	}

	auto ref = fi->second->FrameReferenceAt(FID);

	d_measurementByURI[m->TagCloseUpURI()][m->Type()] = m;
	d_measurements[m->Type()][TID][tddURI][ref.Time()] = m;

	if (m->Type() != Measurement::HEAD_TAIL_TYPE) {
		return;
	}

	d_identifier->SetAntPoseEstimate(std::make_shared<AntPoseEstimate>(ref,
	                                                                   TID,
	                                                                   m->EndFromTag(),
	                                                                   m->StartFromTag()));
}

void Experiment::DeleteMeasurement(const std::string & URI) {
	std::string tddURI;
	FrameID FID;
	TagID tagID;
	MeasurementType::ID MTID;
	Measurement::DecomposeURI(URI,tddURI,FID,tagID,MTID);

	auto tfi = d_universe->TrackingDataDirectories().find(tddURI);
	if ( tfi == d_universe->TrackingDataDirectories().end() ) {
		std::ostringstream oss;
		oss << "Unknown data directory '" << tddURI << "'";
		throw std::invalid_argument(oss.str());
	}
	auto ref = tfi->second->FrameReferenceAt(FID);

	if ( MTID == Measurement::HEAD_TAIL_TYPE ) {
		d_identifier->DeleteAntPoseEstimate(std::make_shared<AntPoseEstimate>(ref,tagID,Eigen::Vector2d(0,0),0.0));
	}


	auto tagCloseUpURI = fs::path(tddURI) / "frames" / std::to_string(FID) / "closeups" / std::to_string(tagID);
	auto fi = d_measurementByURI.find(tagCloseUpURI.generic_string());
	if ( fi == d_measurementByURI.end() ){
		throw std::runtime_error("Unknown measurement '"
		                         + URI
		                         + "'");
	}
	auto ffi = fi->second.find(MTID);
	if ( ffi == fi->second.end() ) {
		throw std::runtime_error("Unknown measurement '"
		                         + URI
		                         + "'");
	}
	fi->second.erase(ffi);
	if ( fi->second.empty() ) {
		d_measurementByURI.erase(fi);
	}
	auto sfi = d_measurements.find(MTID);
	if ( sfi == d_measurements.end() ) {
		throw std::logic_error("Sorting error");
	}
	auto sffi = sfi->second.find(tagID);
	if (sffi == sfi->second.end() ) {
		throw std::logic_error("Sorting error");
	}
	auto sfffi =  sffi->second.find(tddURI);
	if ( sfffi == sffi->second.end() ) {
		throw std::logic_error("Sorting error");
	}
	auto sffffi = sfffi->second.find(ref.Time());
	if ( sffffi == sfffi->second.end() ) {
		throw std::logic_error("Sorting error");
	}

	sfffi->second.erase(sffffi);
	if (sfffi->second.empty() == false) {
		return;
	}
	sffi->second.erase(sfffi);
	if ( sffi->second.empty() == false ) {
		return;
	}
	sfi->second.erase(sffi);
	if ( sfi->second.empty() == false ) {
		return;
	}
	d_measurements.erase(sfi);
}

const Experiment::MeasurementByTagCloseUp &
Experiment::Measurements() const {
	return d_measurementByURI;
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
		throw AlmostContiguousIDContainer<fort::myrmidon::Ant::ID,Ant::Ptr>::UnmanagedObject(AID);
	}

	double cornerWidthRatio = CornerWidthRatio(d_family);

	result.clear();

	auto typedMeasurement = d_measurements.find(type);
	if (typedMeasurement == d_measurements.cend() ) {
		return;
	}


	for ( const auto & ident : afi->second->Identifications() ) {
		double tagSizeMM = d_defaultTagSize;
		if (ident->UseDefaultTagSize() == false ) {
			tagSizeMM = ident->TagSize();
		}
		tagSizeMM *= cornerWidthRatio;

		auto measurementsByTDD = typedMeasurement->second.find(ident->TagValue());
		if ( measurementsByTDD == typedMeasurement->second.cend() ) {
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


MeasurementType::Ptr Experiment::CreateMeasurementType(const std::string & name,MeasurementType::ID MTID) {
	return d_measurementTypes.CreateObject([&name]( MeasurementType::ID MTID ) { return std::make_shared<MeasurementType>(MTID,name); },MTID);
}

void Experiment::DeleteMeasurementType(MeasurementType::ID MTID) {
	auto fi = d_measurementTypes.Objects().find(MTID);
	if ( d_measurements.count(MTID) != 0 ) {
		throw std::runtime_error("Could not remove MeasurementType '" + fi->second->Name() + "' has experiment still contains measurement");
	}

	if (MTID == Measurement::HEAD_TAIL_TYPE ) {
		throw std::invalid_argument("Could not remove default measurement type 'head-tail'");
	}

	d_measurementTypes.DeleteObject(MTID);
}

const MeasurementTypeByID & Experiment::MeasurementTypes() const {
	return d_measurementTypes.Objects();
}

std::pair<Space::Ptr,TrackingDataDirectoryConstPtr>
Experiment::LocateTrackingDataDirectory(const std::string & tddURI) const {
	return d_universe->LocateTrackingDataDirectory(tddURI);
}

Space::Ptr Experiment::LocateSpace(const std::string & spaceName) const {
	return d_universe->LocateSpace(spaceName);
}

AntShapeType::Ptr Experiment::CreateAntShapeType(const std::string & name,
                                                 AntShapeTypeID typeID) {
	return d_antShapeTypes->Create(name,typeID);
}

void Experiment::DeleteAntShapeType(AntShapeTypeID typeID) {
	auto fi = d_antShapeTypes->Find(typeID);
	if ( fi == d_antShapeTypes->End() ) {
		// will throw
		d_antShapeTypes->Delete(typeID);
	}

	for ( const auto & [aID,a] : d_identifier->Ants() ) {
		for ( const auto & [type,c] : a->Capsules() ) {
			if ( type == typeID ) {
				throw std::runtime_error("Could not delete shape type "
				                         + std::to_string(fi->first)
				                         + ":'" + fi->second->Name()
				                         + "': Ant " + Ant::FormatID(aID)
				                         + " has a capsule of this type");
			}
		}
	}
	d_antShapeTypes->Delete(typeID);
}


const AntShapeTypeByID & Experiment::AntShapeTypes() const {
	return d_antShapeTypes->Types();
}

AntShapeTypeContainerConstPtr Experiment::AntShapeTypesConstPtr() const {
	return d_antShapeTypes;
}

fort::myrmidon::priv::AntMetadataConstPtr Experiment::AntMetadataConstPtr() const {
	return d_antMetadata;
}

AntMetadata::Column::Ptr
Experiment::AddAntMetadataColumn(const std::string & name,
                                 AntMetadata::Type type) {
	auto res = AntMetadata::Create(d_antMetadata,name,type);

	for ( const auto & [aID,a] : d_identifier->Ants() ) {
		a->CompileData();
	}

	return res;
}

void Experiment::DeleteAntMetadataColumn(const std::string & name) {
	for ( const auto & [aID,a] : d_identifier->Ants() ) {
		if ( a->DataMap().count(name) != 0 ) {
			throw std::runtime_error("Cannot remove AntMetadataColumn '"
			                         + name
			                         + "': Ant "
			                         + Ant::FormatID(aID)
			                         + " contains data");
		}
	}

	d_antMetadata->Delete(name);

	for ( const auto & [aID,a] : d_identifier->Ants() ) {
		a->CompileData();
	}
}


void Experiment::CloneAntShape(fort::myrmidon::Ant::ID sourceAntID,
                               bool scaleToSize,
                               bool overwriteShapes) {
	auto sourceIt = d_identifier->Ants().find(sourceAntID);
	if ( sourceIt == d_identifier->Ants().cend() ) {
		throw std::invalid_argument("Cannot find and " + Ant::FormatID(sourceAntID) );
	}

	auto source = sourceIt->second;
	if ( source->Capsules().empty() && overwriteShapes == false ) {
		return;
	}

	auto computeSize =
		[this](fort::myrmidon::Ant::ID antID) -> double {
			std::vector<ComputedMeasurement> measurements;
			try {
				ComputeMeasurementsForAnt(measurements,antID,Measurement::HEAD_TAIL_TYPE);
			} catch ( const std::exception & e ) {
				return 0.0;
			}
			double res = 0.0;
			for ( const auto & m : measurements ) {
				res += m.LengthMM / measurements.size();
			}
			return res;
		};

	double baseSize = computeSize(sourceAntID);
	if ( baseSize == 0.0 && scaleToSize == true ) {
		throw std::runtime_error("Ant " + Ant::FormatID(sourceAntID) + " has a size of zero");
	}
	for ( const auto & [aID,ant] : d_identifier->Ants() ) {
		if ( aID == sourceAntID
		     || (overwriteShapes == false && ant->Capsules().empty() == false) ) {
			continue;
		}
		ant->ClearCapsules();
		double scale = 1.0;
		if ( scaleToSize == true ) {
			double antSize = computeSize(aID);
			if ( antSize > 0.0 ) {
				scale = antSize / baseSize;
			}
		}
		for ( const auto & [typeID,sourceCapsule] : source->Capsules() ) {
			auto destCapsule = std::make_shared<Capsule>(scale * sourceCapsule->C1(),
			                                             scale * sourceCapsule->C2(),
			                                             scale * sourceCapsule->R1(),
			                                             scale * sourceCapsule->R2());
			ant->AddCapsule(typeID,destCapsule);
		}
	}
}

InteractionSolver::ConstPtr Experiment::CompileInteractionSolver() const {
	return std::make_shared<InteractionSolver>(d_universe->Spaces(),
	                                           d_identifier->Ants());
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
