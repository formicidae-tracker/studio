#pragma once

#include <fort/myrmidon/utils/FileSystem.hpp>
#include <functional>
#include <vector>


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


template<typename Header,typename Line>
class FileReadWriter {
public:
	typedef std::function<void (const Header &)> HeaderReader;
	typedef std::function<void (const Line &)>   LineReader;
	typedef std::function<void (Line &)>         LineWriter;

	static void Read(const fs::path & path,
	                 std::function<void (const Header & h)> onHeader,
	                 std::function<void (const Line & l)> onLine);
	static void Write(const fs::path & path,
	                  const Header & header,
	                  const std::vector< std::function<void (Line & l)> > & lines);
};

} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort


#include "FileReadWriter.impl.hpp"
