#include "Experiment.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>


#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fm = fort::myrmidon;

using namespace fm::priv;

Experiment::Ptr Experiment::Open(const std::string & filename) {
	int fd =  open(filename.c_str(),O_RDONLY | O_BINARY);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filename + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipInputStream>(file.get());


	fort::myrmidon::pb::FileHeader h;

	bool cleanEOF = false;
	if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(),&cleanEOF) ) {
		throw std::runtime_error("could not parse header message");
	}

	Ptr res = Ptr(new Experiment());

	fort::myrmidon::pb::FileLine line;

	for (;;) {
		if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&line, gunziped.get(), &cleanEOF) ) {
			if ( cleanEOF == true ) {
				return res;
			}
			throw std::runtime_error("Could not read file line");
		}

		if (line.has_experiment() == true ) {
			res->d_experiment.CheckTypeAndMergeFrom(line.experiment());
		}

		if (line.has_antdata() == true ) {
			res->d_ants.push_back(std::make_shared<Ant>(line.release_antdata()));
		}
		line.Clear();
	}
}

void Experiment::Save(const std::string & filename) const {

	int fd =  open(filename.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filename + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644)");
	}

	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());


	fort::myrmidon::pb::FileHeader h;

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
		throw std::runtime_error("could not write header message");
	}

	fort::myrmidon::pb::FileLine line;
	line.set_allocated_experiment(const_cast<fort::myrmidon::pb::Experiment*>(&d_experiment));
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
		throw std::runtime_error("could not write experiment data");
	}
	line.release_experiment();


	for (auto const & a : d_ants) {
		line.set_allocated_antdata(const_cast<fort::myrmidon::pb::AntMetadata*>(a->Metadata()));
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
			throw std::runtime_error("could not write ant metadata");
		}
		line.release_antdata();
	}
}



void Experiment::AddTrackingDataDirectory(const fm::pb::TrackingDataDirectory & path) {

	bool canInsert = d_experiment.datadirectory_size() == 0;
	for (auto iter = d_experiment.mutable_datadirectory()->begin();
	     iter != d_experiment.mutable_datadirectory()->end();
	     ++iter) {
		if ( path.path() == iter->path() ) {
			throw std::invalid_argument("directory '" + path.path() + "' is already present");
		}

		if ( path.endframe() <  iter->startframe() ) {
			canInsert = true;
			continue;
		}

		if ( iter->endframe() >= path.startframe() ) {
			continue;
		}

		auto next = iter + 1;
		if ( next == d_experiment.mutable_datadirectory()->end()  || path.endframe() < next->startframe() ) {
			canInsert = true;
		}
	}

	if ( canInsert == false ) {
		throw std::runtime_error("The frame in the tracking data are overlapping");
	}
	auto toAdd = d_experiment.add_datadirectory();
	toAdd->CheckTypeAndMergeFrom(path);

	std::sort(d_experiment.mutable_datadirectory()->begin(),
	          d_experiment.mutable_datadirectory()->end(),
	          [](const fm::pb::TrackingDataDirectory & a, const fm::pb::TrackingDataDirectory & b ) {
		          return a.startframe() < b.startframe();
	          });
}

void Experiment::RemoveRelativeDataPath(const std::string & path) {
	bool removed = false;
	std::remove_if(d_experiment.mutable_datadirectory()->begin(),
	               d_experiment.mutable_datadirectory()->end(),
	               [&removed,path](const fm::pb::TrackingDataDirectory & a) {
		               if ( a.path() != path ) {
			               return false;
		               }
		               removed = true;
		               return removed;
	               });

	if ( removed == false ) {
		throw std::invalid_argument("Could not find data path '" + path + "'");
	}
}

std::vector<std::string> Experiment::TrackingDataPath() const {
	std::vector<std::string> res;
	res.reserve(d_experiment.datadirectory_size());
	for ( auto const & tdd : d_experiment.datadirectory() ) {
		res.push_back(tdd.path());
	}
	return res;
}

//TODO Remove this helper method
void Experiment::AddAnt(fort::myrmidon::pb::AntMetadata * md) {
	d_ants.push_back(std::make_shared<fm::priv::Ant>(md));
}

const std::vector<fm::priv::Ant::Ptr> & Experiment::Ants() const {
	return d_ants;
}
