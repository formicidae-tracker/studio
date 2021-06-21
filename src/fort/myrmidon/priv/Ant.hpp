#pragma once

#include <memory>

#include <fort/myrmidon/Ant.hpp>
#include <fort/myrmidon/Color.hpp>
#include <fort/myrmidon/Shapes.hpp>

#include <fort/myrmidon/utils/FileSystem.hpp>


#include "Identification.hpp"

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
	// The display state of an Ant
	typedef fort::myrmidon::Ant::DisplayState DisplayState;

	// A pointer to an Ant
	typedef std::shared_ptr<Ant>       Ptr;
	// A pointer to an Ant
	typedef std::shared_ptr<const Ant> ConstPtr;

	// The Constructor for an Ant
	Ant(const AntShapeTypeContainerConstPtr & shapeTypeContainer,
	    const AntMetadataConstPtr & metadata,
	    AntID ID);

	// Its destructor
	~Ant();

	// Gets the TagID identifying this Ant at a given time.
	// @time the <Time> for which we want the identification
	//
	// Gets the <TagID> identifying this Ant at a given. There may not
	// have an identification at this given time, an an exception will be thrown.
	// R version:
	// ```R
	// ant$identifiedBy(fmTimeParse("2020-02-19T15:14:00.000Z"))
	// ```
	//
	// @return a <TagID> that identify this ant at this time if it
	// exists (throw an exception otherwise)
	TagID IdentifiedAt(const Time &) const;

	// The Identification::List associated with this priv::Ant.
	//
	// A sorted <Identification::List> associated with this
	// <priv::Ant>. Adding and removing of <Identification> have to be
	// respectively performed using <Identifier::AddIdentification>
	// and <Identifier::RemoveIdentification>.
	const Identification::ConstList & CIdentifications() const;

	// The Identification::List associated with this priv::Ant.
	//
	// A sorted <Identification::List> associated with this
	// <priv::Ant>. Adding and removing of <Identification> have to be
	// respectively performed using <Identifier::AddIdentification>
	// and <Identifier::RemoveIdentification>.
	const Identification::List & Identifications();


	// Gets the Unique ID of the Ant
	//
	// <Ant> managed by the same <Identifier> are uniquely identified
	// by an Unique ID.
	//
	// @return the unique ID of the Ant
	inline fort::myrmidon::AntID AntID() const {
		return d_ID;
	}

	// Get a formatted ID.
	//
	// @return a string in the format "0xabcd"
	inline std::string FormattedID() const {
		return FormatID(d_ID);
	}

	// Formats an AntID to "0xabcd"
	//
	// @return the <myrmidon::Ant::ID> formatted in hexadecimal "0xabcd"
	static std::string FormatID(fort::myrmidon::AntID ID);

	void AddCapsule(AntShapeTypeID typeID,
	                const std::shared_ptr<Capsule> & capsule);

	const TypedCapsuleList & Capsules() const;

	void DeleteCapsule(size_t index);

	void ClearCapsules();

	const Color & DisplayColor() const;

	void SetDisplayColor(const Color & color);

	DisplayState DisplayStatus() const;

	void SetDisplayStatus(DisplayState s);

	const AntStaticValue & GetValue(const std::string & name,
	                                const Time & time) const;

	void SetValue(const std::string & name,
	              const AntStaticValue & value,
	              const Time & time,
	              bool noOverwrite = false);

	void SetValues(const AntDataMap & map);

	void DeleteValue(const std::string & name,
	                 const Time & time);

	AntStaticValue GetBaseValue(const std::string & name) const;

	const AntDataMap & DataMap();

	const AntConstDataMap & CDataMap() const;

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
	                                                 const Time & time);

	fort::myrmidon::AntID   d_ID;
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
