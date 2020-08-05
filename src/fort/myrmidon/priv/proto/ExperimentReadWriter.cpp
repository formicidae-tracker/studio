#include "ExperimentReadWriter.hpp"

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Ant.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>

#include <fort/myrmidon/ExperimentFile.pb.h>

#include "FileReadWriter.hpp"
#include "IOUtils.hpp"
#include "semver.hpp"
namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

ExperimentReadWriter::ExperimentReadWriter() {}
ExperimentReadWriter::~ExperimentReadWriter() {}

Experiment::Ptr ExperimentReadWriter::DoOpen(const fs::path & filename, bool dataLess) {
	typedef FileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	auto res = Experiment::Create(filename);
	std::vector<Measurement::ConstPtr> measurements;
	ReadWriter::Read(filename,
	                 [filename,dataLess](const pb::FileHeader & h) {
		                 semver::version fileVersion{uint8_t(h.majorversion()),
		                                             uint8_t(h.minorversion()),
		                                             0};

		                 semver::version dataLessSupportBoundaryVersion("0.2.0");
		                 semver::version maxSupportedVersion("0.2.0");
		                 if ( fileVersion >  maxSupportedVersion) {
			                 std::ostringstream os;
			                 os << "Unexpected myrmidon file version " << fileVersion
			                    << " in " << filename
			                    << ": can only works with 0.1.0 or 0.2.0";
			                 throw std::runtime_error(os.str());
		                 }
		                 if ( dataLess == true && fileVersion < dataLessSupportBoundaryVersion ) {
			                 throw std::runtime_error("Uncorrect myrmidon file version "
			                                          + fileVersion.to_string()
			                                          + ": data-less opening is only supported for myrmidon file version above 0.2.0");
		                 }
	                 },
	                 [&measurements,&res,filename,dataLess](const pb::FileLine & line) {
		                 if (line.has_experiment() == true ) {
			                 IOUtils::LoadExperiment(res, line.experiment());
		                 }

		                 if (line.has_antdescription() == true ) {
			                 IOUtils::LoadAnt(res, line.antdescription());
		                 }

		                 if (line.has_measurement() == true && dataLess == false ) {
			                 measurements.push_back(IOUtils::LoadMeasurement(line.measurement()));
		                 }

		                 if (line.has_space() == true ) {
			                 IOUtils::LoadSpace(res,line.space(),dataLess == false);
		                 }
	                 });

	for ( const auto & m : measurements ) {
		res->SetMeasurement(m);
	}

	return res;
}


void ExperimentReadWriter::DoSave(const Experiment & experiment, const fs::path & filepath) {
	typedef FileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	pb::FileHeader h;
	h.set_majorversion(0);
	h.set_minorversion(2);

	std::vector<std::function < void ( pb::FileLine &) > > lines;

	lines.push_back([&experiment](pb::FileLine & line) {
		                IOUtils::SaveExperiment(line.mutable_experiment(),experiment);
	                });

	for ( const auto & [spaceID,space] : experiment.CSpaces() ) {
		lines.push_back([space=space](pb::FileLine & line) {
			                IOUtils::SaveSpace(line.mutable_space(),space);
		                });
	}

	std::vector<fort::myrmidon::Ant::ID> antIDs;
	for (const auto & [ID,a] : experiment.CIdentifier().CAnts() ) {
		antIDs.push_back(ID);
	}
	std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	                                         fort::myrmidon::Ant::ID b) -> bool {
		                                      return a < b;
	                                      });

	for (const auto & ID : antIDs) {
		lines.push_back([&experiment,ID](pb::FileLine & line) {
			                IOUtils::SaveAnt(line.mutable_antdescription(),
			                                 experiment.CIdentifier().CAnts().find(ID)->second);
		                });
	}

	for ( const auto & [uri,measurementByType] : experiment.Measurements() ) {
		for (const auto & [type,m] : measurementByType) {
			lines.push_back([m = m](pb::FileLine & line) {
				                IOUtils::SaveMeasurement(line.mutable_measurement(),m);
			                });
		}
	}
	ReadWriter::Write(filepath,h,lines);
}


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
