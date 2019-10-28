#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include <functional>
#include <vector>
namespace fort {

namespace myrmidon {

namespace utils {


template<typename Header,typename Line>
class ProtobufFileReadWriter {
public:
	static void Read(const fs::path & path,
	                 std::function<void (const Header & h)> onHeader,
	                 std::function<void (const Line & l)> onLine);
	static void Write(const fs::path & path,
	                  const Header & header,
	                  const std::vector< std::function<void (Line & l)> > & lines);
};

}

}

}

#include "ProtobufFileReadWriter.impl.hpp"
