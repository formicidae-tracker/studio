#include "Experiment.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include <sstream>
#include <iomanip>
#include <list>

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>

#include "Ant.hpp"
#include "Identifier.hpp"
#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fm = fort::myrmidon;
namespace fs = std::filesystem;
using namespace fm::priv;

Experiment::Ptr Experiment::Create(const std::filesystem::path & filepath) {
	auto absolutePath = fs::weakly_canonical(filepath);
	auto base = absolutePath;
	base.remove_filename();
	Experiment::Ptr res(new Experiment(absolutePath));

	fs::create_directories(base);
	res->Save(absolutePath);
	return res;
}


void Experiment::Load(const std::filesystem::path & filepath) {
	int fd =  open(filepath.c_str(),O_RDONLY | O_BINARY);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filepath.string() + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipInputStream>(file.get());


	fort::myrmidon::pb::FileHeader h;
	bool cleanEOF = false;
	if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(),&cleanEOF) ) {
		throw std::runtime_error("could not parse header message in '" + filepath.string() + "'");
	}

	if (h.majorversion() != 0 || h.minorversion() != 1 ) {
		std::ostringstream os;
		os << "unexpected version " << h.majorversion() << "." << h.minorversion()
		   << " in " << filepath
		   << " can only works with 0.1";
		throw std::runtime_error(os.str());
	}
	fort::myrmidon::pb::FileLine line;

	for (;;) {
		line.Clear();
		if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&line, gunziped.get(), &cleanEOF) ) {
			if ( cleanEOF == true ) {
				break ;
			}
			throw std::runtime_error("Could not read file line");
		}

		if (line.has_experiment() == true ) {
			d_experiment.Clear();
			d_experiment.CheckTypeAndMergeFrom(line.experiment());
		}

		if (line.has_antdata() == true ) {
			Identifier::LoadAnt(d_identifier,line.antdata());
		}
	}
	//builds the TrackingDataDirectory
	for(auto const & tdd : d_experiment.datadirectory() ) {
		d_dataDirs[tdd.path()] = TrackingDataDirectory::FromSaved(tdd);
	}

}

Experiment::Ptr Experiment::Open(const std::filesystem::path & filepath) {
	Experiment::Ptr res(new Experiment(filepath));
	res->Load(filepath);
	return res;
}

void Experiment::Save(const std::filesystem::path & filepath) const {
	auto basedir = fs::weakly_canonical(d_absoluteFilepath);
	auto newBasedir = fs::weakly_canonical(filepath);
	//TODO: should not be an error.
	if ( basedir.remove_filename() != newBasedir.remove_filename() ) {
		throw std::runtime_error("Changing file directory is not yet supported");
	}

	int fd =  open(filepath.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filepath.string() + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644)");
	}

	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	fort::myrmidon::pb::FileHeader h;
	h.set_majorversion(0);
	h.set_minorversion(1);

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
		throw std::runtime_error("could not write header message");
	}

	fort::myrmidon::pb::FileLine line;

	line.set_allocated_experiment(const_cast<fort::myrmidon::pb::Experiment*>(&d_experiment));
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
		throw std::runtime_error("could not write experiment data");
	}
	line.release_experiment();


	std::vector<fort::myrmidon::Ant::ID> antIDs;
	for (const auto & [ID,a] : d_identifier->Ants() ) {
		antIDs.push_back(ID);
	}
	std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	                                         fort::myrmidon::Ant::ID b) -> bool {
		                                      return a < b;
	                                      });

	for (const auto & ID : antIDs) {
		line.Clear();
		d_identifier->Ants().find(ID)->second->Encode(*line.mutable_antdata());

		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
			throw std::runtime_error("could not write ant metadata");
		}
	}
}

void Experiment::AddTrackingDataDirectory(const TrackingDataDirectory & toAdd) {
	if (d_dataDirs.count(toAdd.Path) != 0 ) {
		throw std::invalid_argument("directory '" + toAdd.Path.string() + "' is already present");
	}

	std::vector<std::string> sortedInTime;
	for(auto const & [path,tdd] : d_dataDirs ) {
		sortedInTime.push_back(path);
	}

	std::sort(sortedInTime.begin(),sortedInTime.end(),
	          [this](const std::string & a, const std::string & b) -> bool{
		          return d_dataDirs[a].StartDate < d_dataDirs[b].StartDate;
	          });
	bool canInsert = d_experiment.datadirectory_size() == 0;
	for(auto iter =  sortedInTime.cbegin();
	    iter != sortedInTime.cend();
	    ++iter) {
		auto const & tdd = d_dataDirs[*iter];
		if ( toAdd.EndDate < tdd.StartDate ) {
			canInsert = true;
			break;
		}

		if ( tdd.EndDate >= toAdd.StartDate ) {
			continue;
		}
		auto next = iter+1;
		if ( next == sortedInTime.cend() ) {
			canInsert = true;
			break;
		}
		auto const & nextTdd = d_dataDirs[*next];

		if ( toAdd.EndDate < nextTdd.StartDate ) {
			canInsert = true;
			break;
		}
	}

	if ( canInsert == false ) {
		throw std::runtime_error("Data in '" + toAdd.Path.string() + "' overlaps in time with existing data");
	}

	d_dataDirs[toAdd.Path] = toAdd;
	auto m = d_experiment.add_datadirectory();
	toAdd.Encode(*m);
}

void Experiment::RemoveTrackingDataDirectory(std::filesystem::path path) {

	if ( path.is_absolute() ) {
		fs::path root = d_absoluteFilepath;
		path = fs::relative(path,root.remove_filename());
	}

	if ( d_dataDirs.count(path) == 0 ) {
		throw std::invalid_argument("Could not find data path '" + path.string() + "'");
	}

	d_dataDirs.erase(path);
	d_experiment.clear_datadirectory();
	for(auto const & tdd : d_dataDirs) {
		auto m = d_experiment.add_datadirectory();
		tdd.second.Encode(*m);
	}

}

const Experiment::TrackingDataDirectoryByPath & Experiment::TrackingDataDirectories() const {
	return d_dataDirs;
}


fort::myrmidon::priv::Ant::Ptr Experiment::CreateAnt() {
	return d_identifier->CreateAnt();
}

void Experiment::DeleteAnt(fort::myrmidon::Ant::ID id) {
	return d_identifier->DeleteAnt(id);
}

const AntByID & Experiment::Ants() const {
	return d_identifier->Ants();
}

Experiment::Experiment(const std::filesystem::path & filepath )
	: d_absoluteFilepath(fs::weakly_canonical(filepath))
	, d_basedir(d_absoluteFilepath)
	, d_identifier(Identifier::Create()) {
	d_basedir.remove_filename();
}



const std::string & Experiment::Name() const {
	return d_experiment.name();
}

void Experiment::SetName(const std::string & name) {
	d_experiment.set_name(name);
}

const std::string & Experiment::Author() const {
	return d_experiment.author();
}

void Experiment::SetAuthor(const std::string & author) {
	d_experiment.set_author(author);
}


const std::string & Experiment::Comment() const {
	return d_experiment.comment();
}

void Experiment::SetComment(const std::string & comment) {
	d_experiment.set_comment(comment);
}

uint8_t Experiment::Threshold() const {
	uint32_t th = d_experiment.threshold();
	if ( th == 0 || th >= 255 ) {
		return 40;
	}
	return th;
}

void Experiment::SetThreshold(uint8_t th) {
	if ( th == 0 || th >= 255 ) {
		th = 40;
	}
	d_experiment.set_threshold(th);
}


const std::filesystem::path & Experiment::AbsolutePath() const {
	return d_absoluteFilepath;
}

const std::filesystem::path & Experiment::Basedir() const {
	return d_basedir;
}


Experiment::TagFamily Experiment::Family() const {
	static std::map<fm::pb::TagFamily,Experiment::TagFamily>
		mapping = {
		           {fm::pb::UNSET,Experiment::TagFamily::Unset},
		           {fm::pb::TAG16H5,Experiment::TagFamily::Tag16h5},
		           {fm::pb::TAG25H9,Experiment::TagFamily::Tag25h9},
		           {fm::pb::TAG36ARTAG,Experiment::TagFamily::Tag36ARTag},
		           {fm::pb::TAG36H10,Experiment::TagFamily::Tag36h10},
		           {fm::pb::TAG36H11,Experiment::TagFamily::Tag36h11},
		           {fm::pb::CIRCLE21H7,Experiment::TagFamily::Circle21h7},
		           {fm::pb::CIRCLE49H12,Experiment::TagFamily::Circle49h12},
		           {fm::pb::CUSTOM48H12,Experiment::TagFamily::Custom48h12},
		           {fm::pb::STANDARD41H12,Experiment::TagFamily::Standard41h12},
		           {fm::pb::STANDARD52H13,Experiment::TagFamily::Standard52h13},
	};
	auto fi = mapping.find(d_experiment.tagfamily());
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid protobuf enum value");
	}
	return fi->second;
}
void Experiment::SetFamily(TagFamily tf) {
	static std::map<Experiment::TagFamily,fm::pb::TagFamily>
		mapping = {
		           {Experiment::TagFamily::Unset,fm::pb::UNSET},
		           {Experiment::TagFamily::Tag16h5,fm::pb::TAG16H5},
		           {Experiment::TagFamily::Tag25h9,fm::pb::TAG25H9},
		           {Experiment::TagFamily::Tag36ARTag,fm::pb::TAG36ARTAG},
		           {Experiment::TagFamily::Tag36h10,fm::pb::TAG36H10},
		           {Experiment::TagFamily::Tag36h11,fm::pb::TAG36H11},
		           {Experiment::TagFamily::Circle21h7,fm::pb::CIRCLE21H7},
		           {Experiment::TagFamily::Circle49h12,fm::pb::CIRCLE49H12},
		           {Experiment::TagFamily::Custom48h12,fm::pb::CUSTOM48H12},
		           {Experiment::TagFamily::Standard41h12,fm::pb::STANDARD41H12},
		           {Experiment::TagFamily::Standard52h13,fm::pb::STANDARD52H13},
	};
	auto fi = mapping.find(tf);
	if ( fi == mapping.end() ) {
		throw std::runtime_error("invalid Experiment::TagFamily enum value");
	}
	d_experiment.set_tagfamily(fi->second);
}
