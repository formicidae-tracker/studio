#pragma once

#include <memory>
#include <myrmidon/utils/FileSystem.hpp>

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

	const Identification::List & Identifications() const;
	void SortAndCheckIdentifications();


	fort::myrmidon::Ant::ID ID() const {
		return d_ID;
	}

	const std::string & FormattedID() const {
		return d_IDStr;
	}

	static std::string FormatID(fort::myrmidon::Ant::ID ID);

	class Accessor {
	private:
		static Identification::List & Identifications(Ant & a);
	public:
		friend class Identifier;
		friend class Identification;
	};

private:
	Ant & operator=(const Ant&) = delete;
	Ant(const Ant&)  = delete;


	uint32_t    d_ID;
	std::string d_IDStr;
	Identification::List d_identifications;
};

} //namespace priv

} // namespace myrmidon

} // namespace fort
