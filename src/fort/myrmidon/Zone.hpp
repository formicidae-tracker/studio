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
// Zone have time valid <Definition>. In most cases there would be a
// single <Definition> for any <Zone> valid for ]-∞,+∞[ <Time>, but it
// is possible to assign different <Definition::Geometry> for
// different time range using multiple <Definition>.
//
// <Definition> are manipulated using <AddDefinition> and
// <EraseDefinition>.
class Zone {
public:
	// A Space-unique ID
	//
	// ID are unique within the same <Space>. Zone from two different
	// <Space> can have the same <ZoneID>.
	typedef uint32_t                    ID;
	// A pointer to a Zone
	typedef std::shared_ptr<Zone>       Ptr;
	// A const pointer to a Zone
	typedef std::shared_ptr<const Zone> ConstPtr;
	// A map of Zone indexed by <ZoneID>
	typedef std::map<ID,Ptr>            ByID;
	// A map of const Zone indexed by <ZoneID>
	typedef std::map<ID,ConstPtr>       ConstByID;

	// Defines the geometry of a <Zone> in <Time>
	//
	// Definition sets for a time range [<Start>,<End>[ a <Geometry>
	// for a <Zone>. nullptr for <Start> or <End> represents -/+∞.
	class Definition {
	public:
		// A Pointer to the Definition
		typedef std::shared_ptr<Definition>       Ptr;
		// A const pointer to the Definition
		typedef std::shared_ptr<const Definition> ConstPtr;
		// A list of Definition
		typedef std::vector<Ptr>                  List;
		// A const list of Definition
		typedef std::vector<ConstPtr>             ConstList;

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
	private:
		typedef std::shared_ptr<priv::ZoneDefinition> PPtr;
		PPtr d_p;
	};


	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Zone> PPtr;

	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Zone directly. They must be build and
	// accessed from <Space>.
	Zone(const PPtr & pZone);


	// Adds a new timed Definition
	// @geometry the shape of the Zone as a <Shape::ConstList>
	// @start the starting validi <Time> for this definition.
	// @end the ending valid <Time> for this definition
	//
	// Adds a new timed <Definition> valid for
	// [<start>,<end>[. nullptr means -/+∞.
	//
	// @return a <Definition::Ptr> for the new <Definition>
	Definition::Ptr AddDefinition(const Shape::ConstList & geometry,
	                              const Time::ConstPtr & start,
	                              const Time::ConstPtr & end);

	// const access to the Zone <Definition>
	//
	// @return a <Definition::ConstList> of <Definition> for this Zone
	Definition::ConstList CDefinitions() const;

	// the Zone <Definition>
	//
	// @return a <Definition::List> of <Definition> for this Zone
	Definition::List Definitions();

	// Removes a <Definition>
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
	void SetName() const;

	// Gets the Zone ID
	//
	// Gets the Zone <ID>. <ID> are unique within a <Space>, but two
	// Zone in different <Space> can have the same <ID>.
	//
	// @return the Zone <ID>x
	ID ZoneID() const;


private:
	PPtr d_p;
};


} // namespace myrmidon
} // namespace fort
