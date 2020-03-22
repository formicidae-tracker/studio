#pragma once

#include <memory>
#include <fort/myrmidon/utils/FileSystem.hpp>

#include <fort/myrmidon/Ant.hpp>

#include "Identification.hpp"
#include "Color.hpp"

#include "TimeMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


// The Object of Interest of any Experiment
//
// <priv::Ant> are the unique object of interest of an <Experiment> they are
// uniquely identified by their <myrmidon::Ant::ID>.
//
// <priv::Identification> relates an <priv::Ant> to a
// <myrmidon::TagID>. <Identifier::Identify> can be used to retrieve
// the <priv::Identification> associated to a <TagID> if it exist in any
// point in time of an <Experiment> defined by a <FramePointer>.
//
// An <priv::Ant> could have multiple <priv::Identification>
// associated with it, but they should not overlap in
// time. <Ant::SortAndCheckIdentifications> could be used to ensure
// this.
class Ant {
public:
	enum class DisplayState {
		VISIBLE = 0,
		HIDDEN  = 1,
		SOLO    = 2
	};
	// A pointer to an Ant
	typedef std::shared_ptr<Ant> Ptr;
	// A pointer to an Ant
	typedef std::shared_ptr<const Ant> ConstPtr;

	// A List of shape
	typedef std::vector<std::pair<AntShapeTypeID,CapsulePtr>> TypedCapsuleList;

	// The Constructor for an Ant
	Ant(const AntShapeTypeContainerConstPtr & shapeTypeContainer,
	    const AntMetadataConstPtr & metadata,
	    fort::myrmidon::Ant::ID ID);

	// Its destructor
	~Ant();

	// The Identification::List associated with this priv::Ant.
	//
	// A sorted <Identification::List> associated with this
	// <priv::Ant>. Adding and removing of <Identification> have to be
	// respectively performed using <Identifier::AddIdentification>
	// and <Identifier::RemoveIdentification>.
	const Identification::List & Identifications() const;

	// Gets the Unique ID of the Ant
	//
	// <Ant> managed by the same <Identifier> are uniquely identified
	// by an Unique ID.
	//
	// @return the unique ID of the Ant
	fort::myrmidon::Ant::ID ID() const {
		return d_ID;
	}

	// Get a formatted ID.
	//
	// @return a string in the format "0xabcd"
	const std::string & FormattedID() const {
		return d_IDStr;
	}

	// Formats an Ant::ID to "0xabcd"
	//
	// @return the <myrmidon::Ant::ID> formatted in hexadecimal "0xabcd"
	static std::string FormatID(fort::myrmidon::Ant::ID ID);

	void AddCapsule(AntShapeTypeID typeID, const CapsulePtr & capsule);

	const TypedCapsuleList & Capsules() const;

	void DeleteCapsule(size_t index);

	void ClearCapsules();

	const Color & DisplayColor() const;

	void SetDisplayColor(const Color & color);

	DisplayState DisplayStatus() const;

	void SetDisplayStatus(DisplayState s);

	const AntStaticValue & GetValue(const std::string & name,
	                               const Time & time);

	void SetValue(const std::string & name,
	              const AntStaticValue & value,
	              const Time::ConstPtr & time);

	void SetValues(const AntDataMap & map);

	void DeleteValue(const std::string & name,
	                 const Time::ConstPtr & time);

	const AntDataMap & DataMap() const;

	void CompileData();

	// C++ shenanigans
	//
	// C++ shenanigan class to give restricted unlimited access to the
	// <Identification::List> to the <Identifier> class.
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


	static std::vector<AntTimedValue>::iterator Find(const AntDataMap::iterator & iter,
	                                                 const Time::ConstPtr & time);

	static bool CompareTime(const AntTimedValue & a, const AntTimedValue &b);

	fort::myrmidon::Ant::ID d_ID;
	std::string             d_IDStr;
	Identification::List    d_identifications;
	TypedCapsuleList        d_capsules;
	Color                   d_displayColor;
	DisplayState            d_displayState;

	AntShapeTypeContainerConstPtr d_shapeTypes;
	AntMetadataConstPtr           d_metadata;

	AntDataMap                          d_data;
	TimeMap<std::string,AntStaticValue> d_compiledData;
};

} //namespace priv
} // namespace myrmidon
} // namespace fort
