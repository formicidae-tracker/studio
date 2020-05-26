#pragma once

#include <map>
#include <memory>

#include "Shapes.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Zone;
// private <fort::myrmidon::priv> implementation
class ZoneDefinition;
}


// const version of ZoneDefinition
//
// Simply a strip down copy of <ZoneDefinition> . Its an helper class
// to support const correctness of object and for language binding
// that does not enforce constness, such as R.
class CZoneDefinition {
public:

	// Gets the geometry of this definition
	//
	// @return a union of <Shape> defining the geometry
	Shape::ConstList Geometry() const;

	// Gets the first valid time of the Definition
	//
	// @return a <Time::ConstPtr> for the first valid
	//         time. nullptr means -∞.
	const Time::ConstPtr & Start() const;

	// Gets the ending valid time of the Definition
	//
	// @return a <Time::ConstPtr> before which the Definition is
	//         valid. nullptr means +∞.
	const Time::ConstPtr & End() const;

	// Opaque implementation pointer
	typedef std::shared_ptr<const priv::ZoneDefinition> ConstPPtr;

	// Private implementation constructor
	// @pDefinition opaque pointer to implementation
	//
	// User cannot build Defoninition directly. They must be build and
	// accessed from <Zone>.
	CZoneDefinition(const ConstPPtr & pDefinition);
private:
	ConstPPtr d_p;
};


// Defines the geometry of a <Zone> in <Time>
//
// ZoneDefinition sets for a time range [<Start>,<End>[ a <Geometry>
// for a <Zone>. nullptr for <Start> or <End> represents -/+∞.
class ZoneDefinition {
public:
	// A list of Definition
	typedef std::vector<ZoneDefinition>  List;
	// A const list of Definition
	typedef std::vector<CZoneDefinition> ConstList;

	// Gets the geometry of this definition
	//
	// @return a union of <Shape> defining the geometry
	Shape::ConstList Geometry() const;

	// Sets the geometry of this definition
	// @shapes a union of <Shape> defining the <Zone> geometry.
	void SetGeometry(const Shape::ConstList & shapes);

	// Gets the first valid time of the Definition
	//
	// @return a <Time::ConstPtr> for the first valid
	//         time. nullptr means -∞.
	const Time::ConstPtr & Start() const;

	// Gets the ending valid time of the Definition
	//
	// @return a <Time::ConstPtr> before which the Definition is
	//         valid. nullptr means +∞.
	const Time::ConstPtr & End() const;

	// Sets the first valid time of the Definition
	// @start the first valid <Time> for the Definition. nullptr
	//        means -∞
	void SetStart(const Time::ConstPtr & start);

	// Sets the last valid time of the Definition
	// @end the <Time> before which the Definition is
	//      valid. nullptr means -∞
	void SetEnd(const Time::ConstPtr & end);

	// Opaque implementation pointer
	typedef std::shared_ptr<priv::ZoneDefinition> PPtr;

	// Private implementation constructor
	// @pDefinition opaque pointer to implementation
	//
	// User cannot build Defoninition directly. They must be build and
	// accessed from <Zone>.
	ZoneDefinition(const PPtr & pDefinition);
private:
	PPtr d_p;
};


// const version of Zone
//
// Simply a strip down copy of <Zone> . Its an helper class
// to support const correctness of object and for language binding
// that does not enforce constness, such as R.
class CZone {
public :
	// const access to the ZoneDefinition
	//
	// @return a <ZoneDefinition::ConstList> of <ZoneDefinition> for this Zone
	ZoneDefinition::ConstList CDefinitions() const;

	// Gets Zone name
	//
	// @return the Zone name
	const std::string & Name() const;

	// Gets the Zone ID
	//
	// Gets the Zone <ZoneID>. <ZoneID> are unique within a <Space>, but two
	// Zone in different <Space> can have the same <ZoneID>.
	//
	// @return the Zone <ZoneID>
	fort::myrmidon::ZoneID ZoneID() const;

	// Opaque pointer for implementation
	typedef std::shared_ptr<const priv::Zone> ConstPPtr;

	// Private implementation constructor
	// @pZone opaque pointer to implementation
	//
	// User cannot build Zone directly. They must be build and
	// accessed from <Space>.
	CZone(const ConstPPtr & pZone);

private:
	ConstPPtr d_p;


};


// A tracking region where interaction are computed.
//
// A Zone defines a tracked area region where interaction can be
// computed. I.e. two <Ant> in different Zone won't report
// interactions.
//
// ## Naming and Identification
//
// Zone are managed by <Space>, and have a unique <ZoneID> within that
// space. However two Zone from different <Space> can share the same
// <ZoneID>. Zone have user definable <Name> but internally only their
// <ZoneID> is used.
//
// By default, any <Ant> is considered to be within the `null` Zone
// with <ZoneID> 0, so all possible interactions are reported. User
// needs to add Zone only to prune unwanted interaction or to be able
// to query where an <Ant> is at any given <Time>.
//
// ## Geometric definition
//
// Zone have time valid <ZoneDefinition>. In most cases there would be
// a single <ZoneDefinition> for any <Zone> valid for ]-∞,+∞[ <Time>,
// but it is possible to assign different <ZoneDefinition::Geometry> for
// different time range using multiple <ZoneDefinition>.
//
// <ZoneDefinition> are manipulated using <AddDefinition> and
// <EraseDefinition>.
class Zone {
public:
	// A Space-unique ID
	//
	// ID are unique within the same <Space>. Zone from two different
	// <Space> can have the same <ZoneID>.
	typedef uint32_t                    ID;
	// A map of Zone indexed by <ZoneID>
	typedef std::map<ID,Zone>            ByID;
	// A map of const CZone indexed by <ZoneID>
	typedef std::map<ID,CZone>           ConstByID;


	// Adds a new timed Definition
	// @geometry the shape of the Zone as a <Shape::ConstList>
	// @start the starting validi <Time> for this definition.
	// @end the ending valid <Time> for this definition
	//
	// Adds a new timed <ZoneDefinition> valid for
	// [<start>,<end>[. nullptr means -/+∞.
	//
	// @return the new <ZoneDefinition>
	ZoneDefinition AddDefinition(const Shape::ConstList & geometry,
	                             const Time::ConstPtr & start,
	                             const Time::ConstPtr & end);

	// const access to the ZoneDefinition
	//
	// @return a <ZoneDefinition::ConstList> of <ZoneDefinition> for this Zone
	ZoneDefinition::ConstList CDefinitions() const;

	// Gets Zone's ZoneDefinition
	//
	// @return a <ZoneDefinition::List> of <ZoneDefinition> for this Zone
	ZoneDefinition::List Definitions();

	// Removes a ZoneDefinition
	// @index the index in <Definitions> to remove.
	void EraseDefinition(size_t index);

	// Gets Zone name
	//
	// @return the Zone name
	const std::string & Name() const;

	// Sets the Zone name
	// @name the wanted new Zone name
	//
	// There are no restrictions on Zone name
	void SetName(const std::string & name);

	// Gets the Zone ID
	//
	// Gets the Zone <ID>. <ID> are unique within a <Space>, but two
	// Zone in different <Space> can have the same <ID>.
	//
	// @return the Zone <ID>x
	ID ZoneID() const;

	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Zone> PPtr;

	// Private implementation constructor
	// @pZone opaque pointer to implementation
	//
	// User cannot build Zone directly. They must be build and
	// accessed from <Space>.
	Zone(const PPtr & pZone);


private:
	PPtr d_p;
};


} // namespace myrmidon
} // namespace fort
