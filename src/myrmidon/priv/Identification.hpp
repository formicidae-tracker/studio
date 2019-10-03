#pragma once

#include <memory>
#include <vector>

#include "FramePointer.hpp"

#include <Eigen/Core>

class IdentificationUTest;

namespace fort {

namespace myrmidon {

namespace priv {

class Identifier;
class Ant;
typedef std::shared_ptr<Ant> AntPtr;
typedef std::shared_ptr<Identifier> IdentifierPtr;

class Identification {
public:
	typedef std::shared_ptr<Identification> Ptr;
	typedef std::vector<Ptr> List;
	static std::pair<List::const_iterator,List::const_iterator>
	SortAndCheckOverlap(List::iterator begin,
	                    List::iterator end);



	void Encode(fort::myrmidon::pb::Identification & pb) const;
	static Ptr FromSaved(const fort::myrmidon::pb::Identification & pb,
	                     const IdentifierPtr & identifier,
	                     const AntPtr & ant);

	const FramePointer::Ptr & Start() const;
	const FramePointer::Ptr & End() const;
	Eigen::Vector2d TagPosition() const;
	double TagAngle() const;
	uint32_t TagValue() const;
	AntPtr Target() const;

	IdentifierPtr ParentIdentifier() const;

	void SetTagPosition(const Eigen::Vector2d & position, double angle);

	class Accessor {
	private:
		static Ptr Create(uint32_t tagValue,
		                  const IdentifierPtr & identifier,
		                  const AntPtr & ant);
		static void SetStart(Identification & identification,
		                     const FramePointer::Ptr & start);
		static void SetEnd(Identification & identification,
		                   const FramePointer::Ptr & end);

	public:
		friend class Identifier;
		friend class ::IdentificationUTest;
	};


private:
	Identification(uint32_t tagValue,
	               const IdentifierPtr & identifier,
	               const AntPtr & ant);
	friend class Ant;
	friend class Identifier;
	friend class ::IdentificationUTest;

	FramePointer::Ptr         d_start;
	FramePointer::Ptr         d_end;
	Eigen::Vector3d           d_position;
	int32_t                   d_tagValue;
	std::weak_ptr<Ant>        d_target;
	std::weak_ptr<Identifier> d_identifier;



};

class OverlappingIdentification : public std::runtime_error {
public:
	OverlappingIdentification(const Identification & a,
	                          const Identification & b);
private:
	static std::string Reason(const Identification & a,
	                          const Identification & b);
};


} // namespace priv

} // namespace myrmidon

} // namespace fort


inline std::ostream & operator<<(std::ostream & out,
                                 const fort::myrmidon::priv::Identification & a) {
	out << "Identification{ID:"
	    << a.TagValue()
	    << ", From:'";
	if (a.Start()) {
		out << *a.Start();
	} else {
		out << "<begin>";
	}
	out << "', To:'";
	if (a.End()) {
		out << *a.End();
	} else {
		out << "<end>";
	}
	return out << "'}";
}
