#include "ExperimentUTest.hpp"

#include "Experiment.hpp"

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <fort-hermes/Header.pb.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif


namespace fm= fort::myrmidon;
namespace fs = std::filesystem;
using namespace fm::priv;

TEST_F(ExperimentUTest,IOTest) {

	try{
		auto e = Experiment::Open(s_testdir / "test.myrmidon" );
		auto tdd = e->TrackingDataPaths();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd["foo.0000"].Path,"foo.0000");

	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}


}

std::filesystem::path ExperimentUTest::s_testdir;

void ExperimentUTest::SetUpTestCase() {
	std::ostringstream os;
	os << "myrmidon-tests-" << getpid();


	EXPECT_NO_THROW({
			auto tmppath = std::filesystem::temp_directory_path() / os.str();
			std::filesystem::create_directories(tmppath / "foo.0000");
			s_testdir = tmppath;

			//creates data
			fm::pb::Experiment e;
			fm::pb::AntMetadata a1;
			fm::pb::AntMetadata a2;

			auto p = e.add_datadirectory();
			p->set_path("foo.0000");
			p->set_startframe(100);
			p->set_endframe(200);
			auto startDate = p->mutable_startdate();
			auto endDate = p->mutable_enddate();
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",startDate),true);
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:42.021-05:00",endDate),true);


			a1.set_id(1);
			a2.set_id(2);

			fm::pb::FileHeader header;

			header.set_majorversion(0);
			header.set_minorversion(1);
			fm::pb::FileLine l;

			auto myrmidonFile = s_testdir / "test.myrmidon";
			int fd = open(myrmidonFile.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644 );
			if ( fd <= 0 ) {
				throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + myrmidonFile.string() + "',O_RDONLY | O_BINARY)");
			}
			auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
			file->SetCloseOnDelete(true);
			auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header, gunziped.get()) ) {
				throw std::runtime_error("could not write header message");
			}

			l.set_allocated_experiment(&e);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
				throw std::runtime_error("could not write experiment data");
			}
			l.release_experiment();

			l.set_allocated_antdata(&a1);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
				throw std::runtime_error("could not write ant data 1");
			}
			l.release_antdata();


			l.set_allocated_antdata(&a2);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
				throw std::runtime_error("could not write ant data 2");
			}
			l.release_antdata();
			ASSERT_NE(gunziped->ByteCount(),0);
			ASSERT_NE(file->ByteCount(),0);
			fsync(fd);
		});
	ASSERT_NE(fs::file_size(s_testdir / "test.myrmidon"),0);
}

void ExperimentUTest::TearDownTestCase() {
	if ( s_testdir.empty() ) {
		return;
	}
	EXPECT_NO_THROW({
			//std::filesystem::remove_all(s_testdir);
		});
}
