#include "TestSetup.hpp"

#include <myrmidon/Experiment.pb.h>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <fort-hermes/Header.pb.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "utils/PosixCall.h"


#ifndef O_BINARY
#define O_BINARY 0
#endif



namespace fm=fort::myrmidon;

// Called before any test activity starts.
void TestSetup::OnTestProgramStart(const ::testing::UnitTest& /* unit_test */)  {
	std::cerr << "Setting up test data" << std::endl;
	std::ostringstream os;
	os << "myrmidon-test-" << getpid();

	auto tmppath = fs::temp_directory_path() / os.str();
	fs::create_directories(tmppath);
	s_testdir = tmppath;
	auto dirs = {"foo.0000","foo.0001","foo.0002","bar.0000"};
	for(auto const & d : dirs) {
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
	tdd.set_startframe(100);
	tdd.set_endframe(105);
	auto startDate = tdd.mutable_startdate();
	auto endDate = tdd.mutable_enddate();
	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",startDate->mutable_timestamp());
	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",endDate->mutable_timestamp());

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




	//creates fake tracking data

	fort::hermes::Header hHeader;
	auto v = hHeader.mutable_version();
	v->set_vmajor(0);
	v->set_vminor(1);
	hHeader.set_type(fort::hermes::Header::Type::Header_Type_File);
	hHeader.set_width(640);
	hHeader.set_height(480);
	fort::hermes::FileLine hLineRo;
	fort::hermes::FileLine hLineFooter;
	auto hRo = hLineRo.mutable_readout();
	auto hFooter = hLineFooter.mutable_footer();


	fd = open( (s_testdir / "foo.0001/tracking.0000.hermes").c_str(),
	           O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	           0644);

	if ( fd <= 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('"
		                        + (s_testdir / "foo.0001/tracking.0000.hermes").string()
		                        + "',O_RDONLY | O_BINARY)");
	}

	auto fileT1 = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunzipedT1 = std::make_shared<google::protobuf::io::GzipOutputStream>(fileT1.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hHeader, gunzipedT1.get()) ) {
		throw std::runtime_error("could not write readout message");
	}

	auto time = hRo->mutable_time();
	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:22.271-05:00",time);
	hRo->set_frameid(5);
	hRo->set_timestamp(0);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT1.get()) ) {
		throw std::runtime_error("could not write readout message");
	}
	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:23.271-05:00",time);
	hRo->set_frameid(6);
	hRo->set_timestamp(120);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT1.get()) ) {
		throw std::runtime_error("could not write readout message");
	}
	hFooter->set_next("tracking.0001.hermes");
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineFooter, gunzipedT1.get()) ) {
		throw std::runtime_error("could not write footer message");
	}


	fd = open( (s_testdir / "foo.0001/tracking.0001.hermes").c_str(),
	           O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	           0644);

	if ( fd <= 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('"
		                        + (s_testdir / "foo.0001/tracking.0001.hermes").string()
		                        + "',O_RDONLY | O_BINARY)");
	}

	auto fileT2 = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunzipedT2 = std::make_shared<google::protobuf::io::GzipOutputStream>(fileT2.get());

	hHeader.set_previous("tracking.0000.hermes");
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hHeader, gunzipedT2.get()) ) {
		throw std::runtime_error("could not write readout message");
	}

	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:24.271-05:00",time);
	hRo->set_frameid(7);
	hRo->set_timestamp(0);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT2.get()) ) {
		throw std::runtime_error("could not write readout message");
	}
	google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:25.271-05:00",time);
	hRo->set_frameid(8);
	hRo->set_timestamp(120);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT2.get()) ) {
		throw std::runtime_error("could not write readout message");
	}

	hFooter->Clear();
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineFooter, gunzipedT2.get()) ) {
		throw std::runtime_error("could not write footer message");
	}
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
