#include "FileReadWriter.hpp"

#include <fcntl.h>

#include <fort/myrmidon/utils/PosixCall.h>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


template<typename Header,typename Line>
inline void
FileReadWriter<Header,Line>::Read(const fs::path & filename,
                                  std::function<void (const Header & h)> onHeader,
                                  std::function<void (const Line & l)> onLine) {
	int fd =  open(filename.c_str(),O_RDONLY | O_BINARY);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filename.string() + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipInputStream>(file.get());

	Header h;
	bool cleanEOF = false;
	if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h, gunziped.get(),&cleanEOF) ) {
		throw std::runtime_error("could not parse header message in '" + filename.string() + "'");
	}
	onHeader(h);

	Line line;
	for (;;) {
		line.Clear();
		if (!google::protobuf::util::ParseDelimitedFromZeroCopyStream(&line, gunziped.get(), &cleanEOF) ) {
			if ( cleanEOF == true ) {
				break ;
			}
			throw std::runtime_error("Could not read file line in '" + filename.string() + "'");
		}
		onLine(line);
	}
}

template<typename Header,typename Line>
inline void
FileReadWriter<Header,Line>::Write(const fs::path & filepath,
                                   const Header & header,
                                   const std::vector< std::function<void (Line & l)> > & lines) {
	int fd =  open(filepath.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644);
	if ( fd  < 0 ) {
		throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + filepath.string() + "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644)");
	}

	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header, gunziped.get()) ) {
		throw std::runtime_error("could not write header message in '" + filepath.string() + "'");
	}

	Line line;
	for( const auto & lineWriters : lines ) {
		line.Clear();
		lineWriters(line);
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(line, gunziped.get()) ) {
			throw std::runtime_error("could not write line data in '" + filepath.string() + "'");
		}
	}
}


} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
