#pragma once

#include <memory>
#include <filesystem>

#include "Experiment.pb.h"
#include "../Ant.hpp"
#include "FramePointer.hpp"

#include "Identification.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class Ant {
public:
	typedef std::shared_ptr<Ant> Ptr;
	Ant(uint32_t ID);
	~Ant();

	Identification::List & Identifications();
	const Identification::List & ConstIdentifications() const;
	void SortAndCheckIdentifications();


	fort::myrmidon::Ant::ID ID() const {
		return d_ID;
	}

	const std::string & FormattedID() const {
		return d_IDStr;
	}

	static std::string FormatID(fort::myrmidon::Ant::ID ID);

private:


	uint32_t    d_ID;
	std::string d_IDStr;
	Identification::List d_identifications;
};

typedef std::unordered_map<fort::myrmidon::Ant::ID,Ant::Ptr> AntByID;

} //namespace priv

} // namespace myrmidon

} // namespace fort
