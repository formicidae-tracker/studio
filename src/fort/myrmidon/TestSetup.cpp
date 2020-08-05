#include "TestSetup.hpp"


#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <fort/hermes/Header.pb.h>

#include <fort/myrmidon/ExperimentFile.pb.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "utils/PosixCall.h"

#include "Time.hpp"
#include "priv/TrackingDataDirectory.hpp"

#include <cmath>
#include <fstream>
#include <random>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <apriltag/apriltag.h>

#include <fort/myrmidon/priv/proto/IOUtils.hpp>

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
std::map<fs::path,std::map<fs::path,std::shared_ptr<uint32_t> > > TestSetup::s_closeUpFiles;

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
		fTime.ToTimestamp(ro->mutable_time());
		ro->set_frameid(i);
		ro->set_timestamp(fTime.MonotonicValue()/1000);
		auto a = ro->add_tags();
		a->set_id(123);
		double phase = 2*M_PI*0.1*fTime.Sub(startTime).Seconds();
		a->set_x(100 + 50*std::cos(phase));
		a->set_y(100 + 50*std::sin(phase));
		a = ro->add_tags();
		a->set_id(124);
		a->set_x(100 + 50*std::cos(phase));
		a->set_y(100 - 50*std::sin(phase));
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

void AddBoundsJittering(std::vector<uint64_t> & bounds, int jitter) {
	uint64_t last = bounds.back();

	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(-4, 4);

	for ( auto & b : bounds ) {
		b += uniform_dist(e1);
	}
	bounds.front() = 0;
	bounds.back() = last;
}

void CreateMovieFiles(std::vector<uint64_t> bounds,
                      const fs::path & basedir) {
	auto lastBoundaryStart = *(bounds.end() - 2);
	AddBoundsJittering(bounds,4);
	static int ii = -2;
	*(bounds.end() - 2) = lastBoundaryStart + 1 + (++ii);
	int i = -1;

	std::random_device r;

    // Choose a random mean between 1 and 6
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> dist(0, 1);

	for(auto iter = bounds.cbegin(); (iter+1) != bounds.cend(); ++iter) {
		std::ostringstream oss;
		oss << std::setw(4) << std::setfill('0') << ++i;

		std::ofstream emptyMovie( (basedir/ ("stream." + oss.str() + ".mp4")).c_str() );

		std::ofstream frameMatching ( (basedir / ("stream.frame-matching." + oss.str() + ".txt")).c_str());

		uint64_t movieID = 0;
		for(uint64_t trackingID = *iter; trackingID < *(iter+1); ++trackingID) {
			// randomly drop 5% of frames
			// if (dist(e1) < 0.05 ) {
			// 	continue;
			// }

			frameMatching << movieID << " " << trackingID << std::endl;
			++movieID;
		}

	}

}

void WriteTagFile(const fs::path & path ) {

	apriltag_family_t * f = tag36h11_create();

	size_t pxSize = 5;
	size_t tagWidth = pxSize * f->total_width;

	cv::Mat img(4*tagWidth,4*tagWidth,CV_8UC1);
	img = 127;


	uint8_t border(255);
	uint8_t inside(0);
	if ( f->reversed_border == true ) {
		border = 0;
		inside = 255;
	}

	auto setPixel = [tagWidth,pxSize,&img](size_t x, size_t y, uint8_t value) {
		                for (size_t px = 0; px < pxSize; ++px) {
			                for (size_t py = 0; py < pxSize; ++py) {
				                size_t xx = x*pxSize + px + tagWidth;
				                size_t yy = y*pxSize + py + tagWidth;
				                img.at<uint8_t>(yy,xx) = value;
			                }
		                }
	                };

	size_t borderSize = f->total_width - f->width_at_border;
	borderSize /=  2;
	for ( size_t tx = 0; tx < f->total_width; ++tx) {
		for ( size_t ty = 0; ty < f->total_width; ++ty) {
			uint8_t color = inside;
			if ( tx < borderSize || tx >= borderSize + f->width_at_border
			     || ty < borderSize || ty >= borderSize + f->width_at_border ) {
				color = border;
			}
			setPixel(tx,ty,color);
		}
	}

	uint64_t code = f->codes[0];
	for ( size_t i = 0; i < f->nbits; ++i) {
		uint8_t color = (code & 1) ?  255 : 0 ;
		code = code >> 1;
		size_t ii = f->nbits - i - 1;
		setPixel(f->bit_x[ii]+borderSize,f->bit_y[ii]+borderSize,color);
	}


	auto center = cv::Point(img.cols / 2,img.rows / 2);
	auto rotMat = cv::getRotationMatrix2D(center,29.0,1.0);

	cv::Mat rotated;
	cv::warpAffine(img,rotated,rotMat,img.size(),cv::INTER_LINEAR,cv::BORDER_CONSTANT,127);

	cv::imwrite(path.string(),rotated);
	tag36h11_destroy(f);
}

void TestSetup::CreateSnapshotFiles(std::vector<uint64_t> bounds,
                                    const fs::path & basedir) {

	auto parentPath = basedir.parent_path();

	AddBoundsJittering(bounds,6);
	bounds.pop_back();

	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> dist(0, 517);

	std::set<int> IDset;
	for ( size_t i = 0; i < 100; ++i) {
		int toAdd;
		do {
			toAdd = dist(e1);
		} while ( IDset.count(toAdd) != 0 );
		IDset.insert(toAdd);
	}

	for ( const auto & b : bounds ) {
		std::vector<int> IDs;
		IDs.reserve(IDset.size());
		for(const auto & ID : IDset) {
			IDs.push_back(ID);
		}
		std::shuffle(IDs.begin(),IDs.end(),e1);

		while(!IDs.empty()) {
			for( int i = 0; i < 9 && !IDs.empty() ; ++i) {
				auto frameID = b + i;
				auto tagID = IDs.back();
				IDs.resize(IDs.size()-1);
				std::ostringstream single,multi;
				single << "ant_" << tagID << "_frame_" << frameID << ".png";
				multi << "frame_" << frameID << ".png";
				auto singleTouchPath = basedir / single.str();
				auto multiTouchPath = basedir / multi.str();
				std::ofstream singleTouch(singleTouchPath.c_str());
				std::ofstream multiTouch(multiTouchPath.c_str());
				s_closeUpFiles[parentPath].insert(std::make_pair(singleTouchPath,std::make_shared<uint32_t>(tagID)));
				s_closeUpFiles[parentPath].insert(std::make_pair(multiTouchPath,std::shared_ptr<uint32_t>()));
			}
		}
	}
	std::ofstream txtTouch( (basedir / "foo.txt").c_str() );
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
	auto foodirs = {"foo.0000","foo.0001","foo.0002","cache-test.0000"};
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
		std::vector<uint64_t> bounds = {0};
		for(size_t i = 0; i < NB_FILES; ++i) {
			auto next = std::make_shared<size_t>(i+1);
			if (i == (NB_FILES-1)) {
				next.reset();
			}
			uint64_t last = (i+1)* 100 - 1;
			s_times[fs::path(d) / HermesFileName(i) ] = WriteHermesFile(s_testdir / d,i,next.get(),
			                             startTime,i*100,last);
			bounds.push_back(last);
			startTime = startTime.Add(10 * Duration::Second
			                          + 103 * Duration::Millisecond
			                          + 14 * Duration::Microsecond);
		}
		startTime = startTime.Add(13 * Duration::Second);

		auto antdir = Basedir() / d / "ants";
		fs::create_directories(antdir);
		std::ofstream touch( (Basedir() / d / "leto-final-config.yml").c_str());


		CreateMovieFiles(bounds, Basedir() / d );
		CreateSnapshotFiles(bounds,antdir);

	}


	startTime = startTime.Add(3 * 24 * Duration::Hour);
	for(auto const & d : bardirs) {
		fs::create_directories(Basedir() / d / "ants");
		WriteTagFile(Basedir() / d / "ants" / "ant_0_frame_0.png");
	}

	//creates data
	fm::pb::Experiment e;

	e.set_author("myrmidon-tests");
	e.set_name("myrmidon test data");
	e.set_comment("automatically generated data");
	e.set_threshold(42);
	e.set_tagfamily(fm::pb::TAG16H5);

	auto mt = e.add_custommeasurementtypes();
	mt->set_id(1);
	mt->set_name("head-tail");

	fm::pb::FileHeader header;

	header.set_majorversion(0);
	header.set_minorversion(2);
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

	fort::myrmidon::pb::Space s;
	s.set_id(1);
	s.set_name("box");
	s.add_trackingdatadirectories("foo.0000");

	l.set_allocated_space(&s);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
		throw std::runtime_error("could not write space data");
	}
	l.release_space();

	for (size_t i = 1; i <=3; ++i) {
		fort::myrmidon::pb::AntDescription a;
		a.set_id(i);
		priv::proto::IOUtils::SaveColor(a.mutable_color(),Palette::Default().At(0));
		l.set_allocated_antdescription(&a);
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
			throw std::runtime_error("could not write ant data " + std::to_string(i));
		}
		l.release_antdescription();
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
