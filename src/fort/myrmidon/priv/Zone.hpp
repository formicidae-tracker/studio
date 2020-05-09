#pragma once


#include "LocatableTypes.hpp"

#include "Shape.hpp"
#include "TimeValid.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Zone;
typedef std::shared_ptr<Zone> ZonePtr;

class ZoneGeometry  {
public:
	typedef std::shared_ptr<const ZoneGeometry> ConstPtr;

	ZoneGeometry(const std::vector<Shape::ConstPtr> & shapes);

	const std::vector<Shape::ConstPtr> & Shapes() const;

	const AABB & GlobalAABB() const;
	const std::vector<AABB> & IndividualAABB() const;

	bool Contains(const Eigen::Vector2d & point ) const;

private:
	std::vector<AABB>            d_AABBs;
	AABB                         d_globalAABB;
	std::vector<Shape::ConstPtr> d_shapes;
};


class ZoneDefinition : public TimeValid {
public:
	typedef std::shared_ptr<ZoneDefinition>       Ptr;
	typedef std::shared_ptr<const ZoneDefinition> ConstPtr;
	typedef std::vector<Ptr>                      List;
	typedef ZoneGeometry                          Geometry;

	ZoneDefinition(const ZonePtr & zone,
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


class Zone : public Identifiable {
public:
	typedef std::shared_ptr<Zone>       Ptr;
	typedef std::shared_ptr<const Zone> ConstPtr;
	typedef uint32_t                    ID;

	typedef ZoneGeometry   Geometry;
	typedef ZoneDefinition Definition;


	static Ptr Create(ID ZID,const std::string & name,const std::string & parentURI);

	Definition::Ptr AddDefinition(const std::vector<Shape::ConstPtr> & shapes,
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
	friend class ZoneDefinition;

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
