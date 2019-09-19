#include "Experiment.priv.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include "utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif


using namespace fort::myrmidion::priv;

Experiment::Ptr Experiment::Open(const std::string & filename) {
	int fd =  open(filename.c_str(),O_RDONLY | O_BINARY);
	if ( fd  < 0 ) {
		throw MYRMIDION_SYSTEM_ERROR(open,errno);
	}

	auto file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipInputStream>(file.get());


	fort::myrmidion::pb::FileHeader h;

	bool cleanEOF = false;
	if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(),&cleanEOF) ) {
		throw std::runtime_error("could not parse header message");
	}

	Ptr res = Ptr(new Experiment());

	fort::myrmidion::pb::FileLine line;

	for (;;) {
		if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(), &cleanEOF) ) {
			if ( cleanEOF == true ) {
				return res;
			}
			throw std::runtime_error("Could not read file line");
		}
		if (line.has_experiment() == true ) {
			res->d_experiment.CheckTypeAndMergeFrom(line.experiment());
		}

		if (line.has_antdata() == true ) {
			res->d_ants.push_back(line.antdata());
		}

	}
}

void Experiment::Save(const std::string & filename) const {
	int fd =  open(filename.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
	if ( fd  < 0 ) {
		throw MYRMIDION_SYSTEM_ERROR(open,errno);
	}

	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());


	fort::myrmidion::pb::FileHeader h;

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
		throw std::runtime_error("could not write header message");
	}

	fort::myrmidion::pb::FileLine line;
	line.set_allocated_experiment(const_cast<fort::myrmidion::pb::Experiment*>(&d_experiment));
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
		throw std::runtime_error("could not write experiment data");
	}
	line.release_experiment();


	for (auto const & a : d_ants) {
		line.set_allocated_antdata(const_cast<fort::myrmidion::pb::AntMetadata*>(&a));
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(h, gunziped.get()) ) {
			throw std::runtime_error("could not write ant metadata");
		}
		line.release_antdata();
	}
}
