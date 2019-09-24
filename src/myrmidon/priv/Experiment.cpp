#include "Experiment.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include <sstream>
#include <iomanip>

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>


#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fm = fort::myrmidon;
namespace fs = std::filesystem;
using namespace fm::priv;


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
			fort::myrmidon::Ant::ID id = line.antdata().id();
			d_antIDs.insert(id);
			d_ants[id] = std::make_shared<Ant>(line.release_antdata());
		}
	}
	d_continuous = false;
	NextAvailableID();
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


	for (auto const & ID : d_antIDs) {
		line.set_allocated_antdata(const_cast<fort::myrmidon::pb::AntMetadata*>(d_ants.find(ID)->second->Metadata()));
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
			throw std::runtime_error("could not write ant metadata");
		}
		line.release_antdata();
	}
}

void Experiment::AddTrackingDataDirectory(const std::filesystem::path & path) {
	auto base = d_absoluteFilepath;
	auto res = TrackingDataDirectory::Open(path,base.remove_filename());
	if (d_dataDirs.count(res.Path) != 0 ) {
		throw std::invalid_argument("directory '" + path.string() + "' is already present");
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
		if ( res.EndDate < tdd.StartDate ) {
			canInsert = true;
			break;
		}

		if ( tdd.EndDate >= res.StartDate ) {
			continue;
		}
		auto next = iter+1;
		if ( next == sortedInTime.cend() ) {
			canInsert = true;
			break;
		}
		auto const & nextTdd = d_dataDirs[*next];

		if ( res.EndDate < nextTdd.StartDate ) {
			canInsert = true;
			break;
		}
	}

	if ( canInsert == false ) {
		throw std::runtime_error("Data in '" + path.string() + "' overlaps in time with existing data");
	}

	d_dataDirs[res.Path] = res;
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
}

const Experiment::TrackingDataDirectoryByPath & Experiment::TrackingDataPaths() const {
	return d_dataDirs;
}


fort::myrmidon::priv::Ant::Ptr Experiment::CreateAnt() {
	auto md = new pb::AntMetadata();
	md->set_id(NextAvailableID());
	auto res = std::make_shared<Ant>(md);
	d_ants[md->id()] =  res;
	d_antIDs.insert(md->id());
	return res;
}

void Experiment::DeleteAnt(fort::myrmidon::Ant::ID id) {
	if ( d_ants.count(id) == 0 ) {
		std::ostringstream os;
		os << "Could not find ant " << id ;
		throw std::out_of_range(os.str());
	}
	if ( id != d_ants.size() ) {
		d_continuous = false;
	}
	d_antIDs.erase(id);
	d_ants.erase(id);
}

const Experiment::AntByID & Experiment::Ants() const {
	return d_ants;
}



Experiment::Experiment(const std::filesystem::path & filepath )
	: d_absoluteFilepath(fs::canonical(filepath)) {
}


fort::myrmidon::Ant::ID Experiment::NextAvailableID() const {
	if ( d_continuous == true ) {
		return d_ants.size() + 1;
	}
	fort::myrmidon::Ant::ID res = 0;
	auto missingIndex = std::find_if(d_antIDs.begin(),d_antIDs.end(),[&res] (fort::myrmidon::Ant::ID toTest ) {
		                                                                 return ++res != toTest;
	                                                                 });

	if (missingIndex == d_antIDs.end() ) {
		const_cast<Experiment*>(this)->d_continuous = true;
		return d_antIDs.size() + 1;
	}
	return res;
}
