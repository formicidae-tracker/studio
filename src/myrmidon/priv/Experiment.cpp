#include "Experiment.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include <sstream>
#include <iomanip>

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>


#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fm = fort::myrmidon;

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
			d_antIDs.insert(line.antdata().id());
			d_ants[line.antdata().id()] = std::make_shared<Ant>(line.release_antdata());
		}
	}
	d_continuous = false;
	NextAvailableID();
	//builds the TrackingDataDirectory
	for(auto const & tdd : d_experiment.datadirectory() ) {
		d_dataDirs[tdd.path()] = tdd;
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


	for (auto const & a : d_ants) {
		line.set_allocated_antdata(const_cast<fort::myrmidon::pb::AntMetadata*>(a.second->Metadata()));
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
			throw std::runtime_error("could not write ant metadata");
		}
		line.release_antdata();
	}
}

void Experiment::AddTrackingDataDirectory(const std::filesystem::path & path) {
	throw std::runtime_error("not yet implemented");
	// bool canInsert = d_experiment.datadirectory_size() == 0;
	// for (auto iter = d_experiment.mutable_datadirectory()->begin();
	//      iter != d_experiment.mutable_datadirectory()->end();
	//      ++iter) {
	// 	if ( path.path() == iter->path() ) {
	// 		throw std::invalid_argument("directory '" + path.path() + "' is already present");
	// 	}

	// 	if ( path.endframe() <  iter->startframe() ) {
	// 		canInsert = true;
	// 		continue;
	// 	}

	// 	if ( iter->endframe() >= path.startframe() ) {
	// 		continue;
	// 	}

	// 	auto next = iter + 1;
	// 	if ( next == d_experiment.mutable_datadirectory()->end()  || path.endframe() < next->startframe() ) {
	// 		canInsert = true;
	// 	}
	// }

	// if ( canInsert == false ) {
	// 	throw std::runtime_error("The frame in the tracking data are overlapping");
	// }
	// auto toAdd = d_experiment.add_datadirectory();
	// toAdd->CheckTypeAndMergeFrom(path);

	// std::sort(d_experiment.mutable_datadirectory()->begin(),
	//           d_experiment.mutable_datadirectory()->end(),
	//           [](const fm::pb::TrackingDataDirectory & a, const fm::pb::TrackingDataDirectory & b ) {
	// 	          return a.startframe() < b.startframe();
	//           });
}

void Experiment::RemoveTrackingDataDirectory(const std::filesystem::path & path) {
	throw std::runtime_error("not yet implemented");
	// bool removed = false;
	// std::remove_if(d_experiment.mutable_datadirectory()->begin(),
	//                d_experiment.mutable_datadirectory()->end(),
	//                [&removed,path](const fm::pb::TrackingDataDirectory & a) {
	// 	               if ( a.path() != path ) {
	// 		               return false;
	// 	               }
	// 	               removed = true;
	// 	               return removed;
	//                });

	// if ( removed == false ) {
	// 	throw std::invalid_argument("Could not find data path '" + path + "'");
	// }
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
		throw std::out_of_range("Could not find Ant");
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

Experiment::TrackingDataDirectory::TrackingDataDirectory()
	: StartFrame(0)
	, EndFrame(0) {
}

std::chrono::system_clock::time_point FromPB( const google::protobuf::Timestamp & ts) {
	return std::chrono::system_clock::from_time_t(google::protobuf::util::TimeUtil::TimestampToTimeT(ts));
}

Experiment::TrackingDataDirectory::TrackingDataDirectory(const pb::TrackingDataDirectory & tdd)
	: Path(tdd.path())
	, StartFrame(tdd.startframe())
	, EndFrame(tdd.endframe())
	, StartDate(FromPB(tdd.startdate()))
	, EndDate(FromPB(tdd.enddate())) {
}


Experiment::Experiment(const std::filesystem::path & filepath )
	: d_basepath(filepath) {
	d_basepath.remove_filename();
}


fort::myrmidon::Ant::ID Experiment::NextAvailableID() const {
	if ( d_continuous == true ) {
		return d_ants.size() + 1;
	}

	fort::myrmidon::Ant::ID res = 0;
	std::find_if(d_antIDs.begin(),d_antIDs.end(),[&res] (fort::myrmidon::Ant::ID toTest ) {
		                                             return ++res != toTest;
	                                             });

	if (res == d_ants.size() + 1 ) {
		const_cast<Experiment*>(this)->d_continuous = true;
	}
	return res;
}
