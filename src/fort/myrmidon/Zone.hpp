#pragma once

#include <vector>
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


// Defines the geometry of a <Zone> in <Time>
//
// ZoneDefinition sets for a time range [<Start>,<End>[ a <Geometry>
// for a <Zone>. nullptr for <Start> or <End> represents -/+∞.
class ZoneDefinition {
public:

	// A pointer to a ZoneDefinition
	typedef std::shared_ptr<ZoneDefinition>       Ptr;
	// A const pointer to a ZoneDefinition
	typedef std::shared_ptr<const ZoneDefinition> ConstPtr;
	// A list of ZoneDefinition
	typedef std::vector<Ptr>                       List;
	// A const list of ZoneDefinition
	typedef std::vector<ConstPtr>                  ConstList;

	// Gets the geometry of this definition
	//
	// R version:
	// ```R
	// zd$geometry()
	// ```
	//
	// @return a union of <Shape> defining the geometry
	Shape::ConstList Geometry() const;

	// Sets the geometry of this definition
	// @shapes a union of <Shape> defining the <Zone> geometry.
	//
	// R version:
	// ```R
	// zd$setGeometry(list(fmCircle(c(x,y),r),...))
	// ```
	void SetGeometry(const Shape::ConstList & shapes);

	// Gets the first valid time of the Definition
	//
	// R version:
	// ```R
	// zd$start()
	// ```
	//
	// @return a <Time> for the first valid time. It can be
	//         <Time::SinceEver>.
	const Time & Start() const;

	// Gets the ending valid time of the Definition
	//
	// R version:
	// ```R
	// zd$end()
	// ```
	//
	// @return a <Time> before which the Definition is valid. It can
	//         be <Time::Forever>.
	const Time & End() const;

	// Sets the first valid time of the Definition
	// @start the first valid <Time> for the Definition. It accepts
	//        <Time::SinceEver>
	//
	// R version:
	// ```R
	// # const_ptr() is needed to convert to fmTimeCPtr
	// zd$setStart(fmTimeParse()$const_ptr())
	// ```
	void SetStart(const Time & start);

	// Sets the last valid time of the Definition
	// @end the <Time> before which the Definition is
	//      valid. It accepts <Time::Forever>
	//
	// R version:
	// ```R
	// # const_ptr() is needed to convert to fmTimeCPtr
	// zd$setEnd(fmTimeParse()$const_ptr())
	// ```
	void SetEnd(const Time & end);

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


// A tracking region where collisions/interactions are computed.
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
	// A map of const Zone indexed by <ZoneID>
	typedef std::map<ID,const Zone>      ConstByID;


	// Adds a new timed Definition
	// @geometry the shape of the Zone as a <Shape::ConstList>
	// @start the starting validi <Time> for this definition.
	// @end the ending valid <Time> for this definition
	//
	// Adds a new timed <ZoneDefinition> valid for [<start>,<end>[. It
	// accepts <Time::SinceEver> and <Time::Forever> for <start> or
	// <end>.
	//
	// R version:
	// ```R
	// z$addDefinition(list(fmCircle(c(x,y),r),...),
	//                 fmTimeParse("XXX")$const_ptr(), # const_ptr() needed to convert to fmTimeCPtr
	//                 fmTimeParse("YYY")$const_ptr()) # const_ptr() needed to convert to fmTimeCPtr
	// ```
	//
	// @return the new <ZoneDefinition>
	ZoneDefinition AddDefinition(const Shape::ConstList & geometry,
	                             const Time & start,
	                             const Time & end);


	// const access to the ZoneDefinition
	//
	// R version:
	// ```R
	// z$cDefinitions()
	// ```
	//
	// @return a <ZoneDefinition::ConstList> of <ZoneDefinition> for this Zone
	ZoneDefinition::ConstList CDefinitions() const;


	// Gets Zone's ZoneDefinition (const overload)
	//
	// R version:
	// ```R
	// z$cDefinitions()
	// ```
	//
	// @return a <ZoneDefinition::ConstList> of <ZoneDefinition> for this Zone
	ZoneDefinition::ConstList Definitions() const;


	// Gets Zone's ZoneDefinition
	//
	// R version:
	// ```R
	// z$definitions()
	// ```
	//
	// @return a <ZoneDefinition::List> of <ZoneDefinition> for this Zone
	ZoneDefinition::List Definitions();

	// Removes a ZoneDefinition
	// @index the index in <Definitions> to remove.
	void EraseDefinition(size_t index);

	// Gets Zone name
	//
	// R version:
	// ```R
	// z$name()
	// ```
	//
	// @return the Zone name
	const std::string & Name() const;

	// Sets the Zone name
	// @name the wanted new Zone name
	//
	// R version:
	// ```R
	// z$setName(name)
	// ```
	//
	// There are no restrictions on Zone name
	void SetName(const std::string & name);

	// Gets the Zone ID
	//
	// Gets the Zone <ID>. <ID> are unique within a <Space>, but two
	// Zone in different <Space> can have the same <ID>.
	//
	// R version:
	// ```R
	// z$zoneID()
	// ```
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
