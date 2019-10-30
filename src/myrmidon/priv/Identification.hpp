#pragma once

#include <memory>
#include <vector>

#include "FramePointer.hpp"

#include <Eigen/Core>

#include "Isometry2D.hpp"

#include "ForwardDeclaration.hpp"

class IdentificationUTest;

namespace fort {

namespace myrmidon {

namespace priv {

// An Identification relates TagID to an Ant
//
// An <priv::Identification> relates <TagID> to an <priv::Ant>.
// <priv::Identification> can be bounded in time through their <Start>
// and <End> <FramePointer::Ptr>. An empty pointer indicates an
// abscence of boundary.
//
// <priv::Identification> also contains geometric information about how a
// physical tag is positionned on the ant. The <SetTagPosition> can be
// used to define the position of the Tag in the actual Ant reference
// frame. The Helper method <ComputeTagToAntTransform> can be used to
// compute these values from a Ant head / tail position and a Tag
// position in any reference (but they have to be the same for both
// ofc).
//
// TODO: It seems weird that sometimes we use <Isometry2D>, and some
// time pair of point and sometimes translation and orientation. A
// bbit of uniformity would help.

class Identification {
public:
	// A Pointer to an Identification
	typedef std::shared_ptr<Identification> Ptr;
	// A List of Identification
	typedef std::vector<Ptr> List;

	// Sorts a list of identification and returns any overlap
	// @begin an iterator to the start of the range
	// @end an iterator the end of the range
	// @return the first pair of Identification overlapping in time. a
	//         pair of empty pointer if none.
	//
	static std::pair<List::const_iterator,List::const_iterator>
	SortAndCheckOverlap(List::iterator begin,
	                    List::iterator end);

	// Gets the TagID of this Identification
	// @return <TagID> this <priv::Identification> refers to.
	uint32_t TagValue() const;

	// Sets the starting validity time for this Identification
	// @start the starting time, could be an empty pointer to remove
	//        any boundary
	//
	// Sets the starting validity time for this <Identification>. This
	// methods will throw any <OverlappingIdentification> if such
	// modification will create any collision for the same <TagID> or
	// the same <priv::Ant>. In such a case the boundaries remain
	// unchanged.
	void SetStart(const FramePointer::Ptr & start);

	// Sets the ending validity time for this Identification
	// @end the ending time, could be an empty pointer to remove
	//        any boundary
	//
	// Sets the ending validity time for this <Identification>. This
	// methods will throw any <OverlappingIdentification> if such
	// modification will create any collision for the same <TagID> or
	// the same <priv::Ant>. In such a case the boundaries remain
	// unchanged.
	void SetEnd(const FramePointer::Ptr & end);

	// Gets the starting validity time
	// @return the first valid time or an empty pointer if no boundary.
	FramePointer::ConstPtr Start() const;

	// Get the ending validity time
	// @return the last valid time or an empty pointer if no boundary.
	FramePointer::ConstPtr End() const;

	// Sets the Tag Position relative to the Ant
	// @position the translation from the <priv::Ant> origin to the Tag center.
	// @angle the angle between the <priv::Ant> and the tag.
	void SetTagPosition(const Eigen::Vector2d & position, double angle);

	// Gets the tag position in the Ant reference frame
	// @return a vector from the <priv::Ant> origin to the tag center
	Eigen::Vector2d TagPosition() const;
	// Gets the tag rotation
	// @return the angle in radian between the <priv::Ant> reference and the tag.
	double TagAngle() const;

	// Gets the transformation from the Ant reference to the Tag reference
	// @return an <Isometry2D> that performs the transformation from
	//         the Ant reference to the tag one.
	//
	// Given a tag position tp and angle ta in image space, one would
	// be able to get the actual position and orientation of the ant
	// in image space by computing : `Isometry2D(tp,ta) *
	// AntToTagTransform()`, and using <Isometry2D::angle> and
	// <Isometry2D::translation>.
	inline const Isometry2Dd & AntToTagTransform() const {
		return d_antToTag;
	}

	// Gets the identified Ant
	// @return an <Ant::Ptr> to the identified Ant
	//
	// Gets the identified <priv::Ant>. It can throw
	// <DeletedReference> if by any bug the referenced object has been
	// destroyed.
	AntPtr Target() const;

	// Gets the parent Identifier
	// @return an <Indentifier::Ptr> to the parent <Identifier>
	//
	// Gets the parent <Identifier>. It can throw
	// <DeletedReference> if by any bug the referenced object has been
	// destroyed.
	IdentifierPtr ParentIdentifier() const;

	// Tests if this identification targets a given point in time.
	// @frame the point in time to test for
	// @return true if this Identification is valid for this point in
	// time.
	inline bool TargetsFrame(const FramePointer & frame ) const {
		return (!d_start || *d_start <= frame) &&
			(!d_end || frame <= *d_end);
	}

	// Computes the TagToAntTransform
	// @result the result transform
	// @tagPosition the position to the tag in image space
	// @tagAngle the orientation of the tag in image space
	// @head the head of the <priv::Ant> in image space
	// @tail the tail of the <priv::Ant> in image space
	//
	// Helper function to compute <TagPosition> and <TagAngle>
	static void ComputeTagToAntTransform(Isometry2Dd & result,
	                                     const Eigen::Vector2d & tagPosition, double tagAngle,
	                                     const Eigen::Vector2d & head,
	                                     const Eigen::Vector2d & tail);


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
	Identification & operator=(const Identification&) = delete;
	Identification(const Identification&)  = delete;

	Identification(uint32_t tagValue,
	               const IdentifierPtr & identifier,
	               const AntPtr & ant);

	void SetBound(const FramePointer::Ptr & start,
	              const FramePointer::Ptr & end);
	friend class Ant;
	friend class Identifier;
	friend class ::IdentificationUTest;

	FramePointer::Ptr         d_start;
	FramePointer::Ptr         d_end;
	Isometry2Dd               d_antToTag;

	int32_t                   d_tagValue;
	std::weak_ptr<Ant>        d_target;
	std::weak_ptr<Identifier> d_identifier;

};
// An std::exception when tow Identification overlaps in time.
//
// Two <priv::Identification> overlaps in time if they have
// overlapping boundary and they either use the same <TagID> or targets
// the same <priv::Ant>. This is an invariant condition that should
// never happen and modification that will break this invariant will
// throw this exception.
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


// Formats an Identification to an std::ostream
// @out the stream to format to
// @a the <fort::myrmidon::priv::Identification> to format
// @return a reference to <out>
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
