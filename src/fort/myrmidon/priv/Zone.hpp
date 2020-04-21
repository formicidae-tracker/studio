#pragma once


#include "LocatableTypes.hpp"

#include "Shape.hpp"
#include "TimeValid.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class Zone : public Identifiable {
public:
	typedef std::shared_ptr<Zone>       Ptr;
	typedef std::shared_ptr<const Zone> ConstPtr;
	typedef uint32_t                    ID;

	class Geometry  {
	public:
		typedef std::shared_ptr<const Geometry> ConstPtr;

		Geometry(const std::vector<Shape::ConstPtr> & shapes);

		const std::vector<Shape::ConstPtr> & Shapes() const;

		const AABB & GlobalAABB() const;
		const std::vector<AABB> & IndividualAABB() const;

	private:
		std::vector<AABB>            d_AABBs;
		AABB                         d_globalAABB;
		std::vector<Shape::ConstPtr> d_shapes;
	};

	class Definition : public TimeValid {
	public:
		typedef std::shared_ptr<Definition> Ptr;
		typedef std::vector<Ptr>   List;

		Definition(const Zone::Ptr & zone,
		           Geometry::ConstPtr geometry,
		           const Time::ConstPtr & start,
		           const Time::ConstPtr & end);

		const Geometry::ConstPtr & GetGeometry() const;

		void SetGeometry(const Geometry::ConstPtr & geometry);

		const Time::ConstPtr & Start() const;

		const Time::ConstPtr & End() const;

		void SetStart(const Time::ConstPtr & start);

		void SetEnd(const Time::ConstPtr & end);

	private:
		void SetBound(const Time::ConstPtr & start, const Time::ConstPtr & end);

		std::weak_ptr<Zone> d_zone;
		Geometry::ConstPtr  d_geometry;
	};

	static Ptr Create(ID ZID,const std::string & name,const std::string & parentURI);

	Definition::Ptr AddDefinition(const Geometry::ConstPtr &,
	                              const Time::ConstPtr & start,
	                              const Time::ConstPtr & end);


	bool NextFreeTimeRegion(Time::ConstPtr & start,Time::ConstPtr & end) const;

	const Definition::List & Definitions() const;

	void EraseDefinition(size_t index);

	const std::string & Name() const;

	void SetName(const std::string & name);

	const std::string & URI() const;

	ID ZoneID() const;

	Geometry::ConstPtr AtTime(const Time & t);

private:
	Zone(ID ZID,const std::string & name, const std::string & parentURI);

	ID                  d_ZID;
	std::weak_ptr<Zone> d_itself;
	std::string         d_name,d_URI;
	Definition::List    d_definitions;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Zone::Definition & definition);
