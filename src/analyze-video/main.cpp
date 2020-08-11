#include <iostream>
#include <stdexcept>

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace fmp = fort::myrmidon::priv;

typedef uint8_t fontchar[16];
static fontchar fontdata[256];


char ExtractDigit(const cv::Mat & frame) {
	if ( frame.cols != 8 || frame.rows != 16 ) {
		throw std::runtime_error("Could not extract digit: not right size");
	}
	std::string expected,got;

	fontchar actual;
	for ( size_t iy = 0 ; iy < 16; ++iy) {
		actual[iy] = 0;
		for (size_t ix = 0; ix < 8; ++ix) {
			auto value  = frame.at<cv::Vec3b>(iy,ix);
			uint8_t gray = (int(value[0]) + int(value[1]) + int(value[2])) / 3;
			if ( gray > 127 ) {
				actual[iy] |=  1 << (7-ix);
			}
		}
	}

	for ( size_t i = 0; i  < 10; ++i) {
		char c = '0'+i;
		size_t errors = 0;
		size_t iy = 0;
		for (  ; iy < 16; ++iy) {
			if ( fontdata[c][iy] != actual[iy] ) {
				break;
			}
		}
		if ( iy == 16 ) {
			return c;
		}
	}
	throw std::runtime_error("COuld not extract digit");
}


uint64_t ExtractFrameNumber(const cv::Mat & frame) {
	uint64_t res = 0;
	for ( size_t i = 0; i < 8; ++i) {
		cv::Mat charLabel = frame(cv::Rect(9*(6+i),0,8,16));
		char digit = ExtractDigit(charLabel);

		res *= 10;
		res += digit - '0';
	}
	return res;
}

void ProcessSegment( const fmp::MovieSegment::ConstPtr & segment) {
	cv::namedWindow ("bad frames");
	auto capture = cv::VideoCapture(segment->AbsoluteFilePath().c_str());

	cv::Mat frame;
	while(true) {
		capture >> frame;
		if ( frame.empty() ) {
			return;
		}
		uint64_t actualFrameID;
		uint64_t movieFrame = capture.get(cv::CAP_PROP_POS_FRAMES) - 1;

		try {
			actualFrameID = ExtractFrameNumber(frame);
		} catch (const std::exception & e ) {
			std::cerr << "Could not extract frame number for " << movieFrame << std::endl;
			continue;
		}

		uint64_t parsedFrameID = segment->ToTrackingFrameID(movieFrame);
		if ( parsedFrameID != actualFrameID ) {
			std::cout << " Extracted " << actualFrameID
			          << " for Frame "  << movieFrame
			          << " parsed: " << parsedFrameID
			          << std::endl;
		}
	}

}

void Execute(int argc, char ** argv) {
	if ( argc != 2 ) {
		throw std::invalid_argument("Need a directory");
	}
	FILE * f = fopen("vga.fon","rb");
	if ( !f ) {
		throw std::runtime_error("Could not open vga.fon");
	}
	int res = fread(fontdata,4096,1,f);
	if ( res != 4096 ) {
		throw std::runtime_error("Could not read full font data");
	}
	fclose(f);

	auto tdd = fmp::TrackingDataDirectory::Open(argv[1],"./");

	for ( const auto & [ref,segment] : tdd->MovieSegments().Segments() ) {
		ProcessSegment(segment);
	}
}


int main (int argc, char ** argv) {
	try {
		Execute(argc,argv);
	} catch (const std::exception & e) {
		std::cerr << "Unhandled exception: " << e.what() << std::endl;
		return 1;
	}
}
