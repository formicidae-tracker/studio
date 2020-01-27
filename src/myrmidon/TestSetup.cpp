#include "TestSetup.hpp"

#include <myrmidon/File.pb.h>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <fort-hermes/Header.pb.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "utils/PosixCall.h"

#include "Time.hpp"
#include "priv/TrackingDataDirectory.hpp"

#include <cmath>
#include <fstream>


#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace fort::myrmidon;

std::string HermesFileName(size_t i) {
	std::ostringstream os;
	os << "tracking." << std::setw(4) << std::setfill('0') << i << ".hermes";
	return os.str();
}

std::map<fs::path,std::pair<Time,Time>> TestSetup::s_times;

std::pair<Time,Time> WriteHermesFile(const fs::path & basepath, size_t number, size_t * next,
                                     const Time & startTime,
                                     uint64_t start, uint64_t end) {
	fort::hermes::Header hHeader;
	auto v = hHeader.mutable_version();
	v->set_vmajor(0);
	v->set_vminor(1);
	hHeader.set_type(fort::hermes::Header::Type::Header_Type_File);
	hHeader.set_width(640);
	hHeader.set_height(480);

	fs::path filename = basepath / HermesFileName(number);
	int fd = open( filename.c_str(),
	           O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	           0644);

	if ( fd <= 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('"
		                        + filename.string()
		                        + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hHeader, gunziped.get()) ) {
		throw std::runtime_error("could not write header");
	}

	fort::hermes::FileLine lineRO,lineFooter;
	Time fTime;
	for( uint64_t i = start; i <= end; ++i) {
		fTime = startTime.Add((i-start)* 100 * fort::myrmidon::Duration::Millisecond);
		auto ro = lineRO.mutable_readout();
		ro->Clear();
		fTime.ToTimestamp(*ro->mutable_time());
		ro->set_frameid(i);
		ro->set_timestamp(fTime.MonotonicValue()/1000);
		auto a = ro->add_tags();
		a->set_id(123);
		a->set_x(100 + 50*std::cos(2*M_PI*fTime.Sub(startTime).Seconds()));
		a->set_y(100 + 50*std::sin(2*M_PI*fTime.Sub(startTime).Seconds()));
		if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(lineRO, gunziped.get()) ) {
			throw std::runtime_error("could not write readout");
		}
	}
	auto footer = lineFooter.mutable_footer();
	if ( next != NULL ) {
		footer->set_next(HermesFileName(*next));
	}
	if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(lineFooter, gunziped.get()) ) {
		throw std::runtime_error("Could not write footer");
	}

	auto resStart = Time::FromTimestampAndMonotonic(startTime.ToTimestamp(),
	                                                (startTime.MonotonicValue() / 1000) * 1000,
	                                                startTime.MonoID());

	fTime = Time::FromTimestampAndMonotonic(fTime.ToTimestamp(),
	                                        (fTime.MonotonicValue()/1000)*1000,
	                                        fTime.MonoID());
	fTime = fTime.Add(1);
	return std::make_pair(resStart,fTime);
}


namespace fm=fort::myrmidon;

// Called before any test activity starts.
void TestSetup::OnTestProgramStart(const ::testing::UnitTest& /* unit_test */)  {
	std::cerr << "Setting up test data" << std::endl;
	std::ostringstream os;
	os << "myrmidon-test-" << getpid();

	auto tmppath = fs::temp_directory_path() / os.str();
	fs::create_directories(tmppath);
	s_testdir = tmppath;
	auto foodirs = {"foo.0000","foo.0001","foo.0002"};
	auto bardirs = {"bar.0000"};

	google::protobuf::Timestamp ts;
	google::protobuf::util::TimeUtil::FromString("2019-11-02T10:00:20.021+01:00",&ts);
	auto startTime = Time::FromTimestampAndMonotonic(ts,
	                                                 123456,
	                                                 priv::TrackingDataDirectory::GetUID(s_testdir /"foo.0001"));
	auto saveStartTime = startTime;

	for(auto const & d : foodirs) {
		fs::create_directories(Basedir() / d);
		startTime = Time::FromTimestampAndMonotonic(startTime.ToTimestamp(),
		                                            startTime.MonotonicValue(),
		                                            priv::TrackingDataDirectory::GetUID(s_testdir/ d));
		const static size_t NB_FILES = 10;
		for(size_t i = 0; i < NB_FILES; ++i) {
			auto next = std::make_shared<size_t>(i+1);
			if (i == (NB_FILES-1)) {
				next.reset();
			}

			s_times[fs::path(d) / HermesFileName(i) ] = WriteHermesFile(s_testdir / d,i,next.get(),
			                             startTime,i*100,(i+1)* 100 - 1);
			startTime = startTime.Add(10 * Duration::Second
			                          + 103 * Duration::Millisecond
			                          + 14 * Duration::Microsecond);
		}
		startTime = startTime.Add(13 * Duration::Second);

		fs::create_directories(Basedir() / d / "ants");
		std::ofstream touch( (Basedir() / d / "leto-final-config.yml").c_str());
	}
	startTime = startTime.Add(3 * 24 * Duration::Hour);
	for(auto const & d : bardirs) {
		fs::create_directories(Basedir() / d);
	}


	//creates data
	fm::pb::Experiment e;

	e.set_author("myrmidon-tests");
	e.set_name("myrmidon test data");
	e.set_comment("automatically generated data");
	e.set_threshold(42);
	e.set_tagfamily(fm::pb::TAG16H5);


	fm::pb::TrackingDataDirectory tdd;
	tdd.set_path("foo.0000");
	tdd.set_startframe(0);
	tdd.set_endframe(99);
	saveStartTime.ToTimestamp(*tdd.mutable_startdate()->mutable_timestamp());
	tdd.mutable_startdate()->set_monotonic(saveStartTime.MonotonicValue());
	auto endTime = saveStartTime.Add(99*100*Duration::Millisecond);
	endTime.ToTimestamp(*tdd.mutable_enddate()->mutable_timestamp());
	tdd.mutable_enddate()->set_monotonic(endTime.MonotonicValue());

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

	for (size_t i = 1; i <=3; ++i) {
		fort::myrmidon::pb::AntMetadata a;
		a.set_id(i);
		l.set_allocated_antdata(&a);
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
			throw std::runtime_error("could not write ant data 1");
		}
		l.release_antdata();
	}

	l.set_allocated_trackingdatadirectory(&tdd);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
		throw std::runtime_error("could not write tracking directory data");
	}
	l.release_trackingdatadirectory();


}

// Called after all test activities have ended.
void TestSetup::OnTestProgramEnd(const ::testing::UnitTest& unit_test) {
	if (Basedir().empty()) {
		return;
	}
	std::cerr << "removing testdata in " << Basedir() << std::endl;
	fs::remove_all(Basedir());
	s_testdir = "";
}

// Called before a test starts.
void TestSetup::OnTestStart(const ::testing::TestInfo& test_info) {}

// Called after a failed assertion or a SUCCEED() invocation.
void TestSetup::OnTestPartResult(const ::testing::TestPartResult& test_part_result) {}

// Called after a test ends.
void TestSetup::OnTestEnd(const ::testing::TestInfo& test_info) {}

fs::path TestSetup::s_testdir;
