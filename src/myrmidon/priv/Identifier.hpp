#pragma once

#include <memory>
#include <unordered_map>
#include <set>

#include "../Ant.hpp"
#include "../Time.hpp"

#include "Types.hpp"

#include "ForwardDeclaration.hpp"

#include "DeletedReference.hpp"

namespace fort {


namespace myrmidon {

namespace pb {
class AntMetadata;
}


namespace priv {

// An Identifier identifies Ants through Identification
//
// The <Identifier> is responsible to keep track in the the
// <priv::Experiment> of the <priv::Ant> i and their
// Identification. Both <priv::Ant> and <priv::Identification> need to
// be created and deleted through its interface as it the only way to
// make sure that we respect the non-<OverlappingIdentification>
// invariant in the library.
class Identifier {
public:
	// A Pointer to an Identifier
	typedef std::shared_ptr<Identifier> Ptr;

	// Creates a new Identifier
	// @return a pointer to an Identifier
	//
	// Creates a new <priv::Identifier>. It has to be shared_ptr as
	// <priv::identification> keeps a std::weak_ptr to this object. and the
	static Ptr Create();


	// For unit test purpose only
	static Identifier Invalid();

	// A self-referencing pointer
	//@return a pointer to itself.
	Ptr Itself() const;

	// A default asking for the next available ID
	const static fort::myrmidon::Ant::ID NEXT_AVAILABLE_ID = 0;

	// Create an Ant
	// @ID the desired ID
	//
	// Creats a new Ant with the iven ID. It will throw an
	// <AlreadyExistingAnt> if the ID is already used. If
	// NEXT_AVAILABLE_ID is used, a unique ID will be automatically
	// chosen.
	AntPtr CreateAnt(fort::myrmidon::Ant::ID ID = NEXT_AVAILABLE_ID);

	// Deletes an Ant
	// @ID the <priv::Ant> to delete
	//
	// Deletes an <priv::Ant> from the Identifier. It should have no
	// Identification targetting her otherwise an exception will be
	// thrown.
	void DeleteAnt(fort::myrmidon::Ant::ID );

	// Gets the Ants in the Identifier
	// @return the map of <priv::Ant> by their <myrmidon::Ant::ID>
	const AntByID & Ants() const;

	// Adds a new Identification
	// @id the targeted <priv::Ant>
	// @tagValue the used TagID
	// @start the first valid time for this <priv::Identification>
	// @end the last valid time for this <proiv::Identification>
	//
	// Adds a new Identification. It may throw
	// <OverlappingIdentification> if any exists for the desired
	// <priv::Ant> or <TagID>.
	IdentificationPtr AddIdentification(fort::myrmidon::Ant::ID id,
	                                    TagID tagValue,
	                                    const Time::ConstPtr & start,
	                                    const Time::ConstPtr & end);

	// Removes an Identification
	// @ident the <priv::Identification> to remove
	//
	// Any <priv::Identification> targetting a given <priv::Ant>
	// should be deleted before removing the <priv::Ant>
	void DeleteIdentification(const IdentificationPtr & ident);

	// An exeption when an Ant is not managed by this Identifier
	class UnmanagedAnt : public std::runtime_error {
	public:
		UnmanagedAnt(fort::myrmidon::Ant::ID id) noexcept;
		virtual ~UnmanagedAnt() noexcept {};
	};
	// An exeption when a TagID is not managed by this Identifier
	class UnmanagedTag : public std::runtime_error {
	public:
		UnmanagedTag(TagID tagValue) noexcept;
		virtual ~UnmanagedTag() noexcept {};
	};
	// An exeption when an Identification is not managed by this Identifier
	class UnmanagedIdentification : public std::runtime_error {
	public:
		UnmanagedIdentification(const Identification & ident) noexcept;
		virtual ~UnmanagedIdentification() noexcept {};
	};
	// An exeption when an Ant is already existing
	class AlreadyExistingAnt : public std::runtime_error {
	public:
		AlreadyExistingAnt(fort::myrmidon::Ant::ID id) noexcept;
		virtual ~AlreadyExistingAnt() noexcept {};
	};

	class Accessor {
	private:
		static IdentificationList & IdentificationsForTag(Identifier & identifier,TagID tagID);
	public:
		friend class Identification;
	};

	// Performs invarinat checks for two tags sharing the same ant or the same TagID
	static void SortAndCheck(IdentificationList & tagSibling,
	                         IdentificationList & antSibling);

	// Identifies an ant from a point in time and a TagID
	// @tag <TagID> to look for
	// @frame the frame to look for
	// @return an <Identification::Ptr> if any exists for that tag at this point in time.
	IdentificationPtr Identify(TagID tag,const Time & frame) const;

	// Return the first next frame if any where tag is not used
	Time::ConstPtr UpperUnidentifiedBound(TagID tag, const Time & t) const;
	// Return the first previoys frame if any where tag is not used
	Time::ConstPtr LowerUnidentifiedBound(TagID tag, const Time & t) const;

	// Returns the number of time a given tag is used.
	size_t UseCount(TagID tag) const;

	// Found the largest time range where a <TagID> is unused.
	// @start is set to the first frametime the tag is unused, or an
	//        empty pointer if the tag isn't used before <t>
	// @end   set the last time where the tag is unused, or an
	//        empty pointer if the tag isn't used after <t>
	// @tag the <TagID> to inquire for
	// @t the <Time> designating the point in time we want a free range.
	// @return true if such a range was found, false if <tag> is already used at time <t>
	//
	// Try to find the largest range where the <tag> is not used,
	// containing the <Time> <f>. If the tag is actually used at this
	// time, returns false. Otherwise returns true and sets <start>
	// and <end> accordingly. Note that a reset <time::ConstPtr> means
	// +/-∞.
	bool FreeRangeContaining(Time::ConstPtr & start,
	                         Time::ConstPtr & end,
	                         TagID tag, const Time & t) const;


	void SetAntPoseEstimate(const AntPoseEstimateConstPtr & tpe);

private:
	class AntPoseTimeComparator {
	public:
		bool operator() (const AntPoseEstimateConstPtr & a,
		                 const AntPoseEstimateConstPtr & b);

	};

	void UpdateIdentification(Identification & identification);

	typedef std::set<fort::myrmidon::Ant::ID>            SetOfID;
	typedef std::unordered_map<TagID,IdentificationList> IdentificationByTagID;

	typedef std::set<AntPoseEstimateConstPtr,
	                 AntPoseTimeComparator>     AntPoseEstimateList;
	typedef std::map<TagID,AntPoseEstimateList> AntPoseEstimateByTagID;

	Identifier();
	Identifier(const Identifier&) = delete;
	Identifier & operator=(const Identifier&) = delete;

	fort::myrmidon::Ant::ID NextAvailableID();


	std::weak_ptr<Identifier> d_itself;

	AntByID d_ants;
	SetOfID d_antIDs;
	bool    d_continuous;

	IdentificationByTagID  d_identifications;
	AntPoseEstimateByTagID d_tagPoseEstimates;

};



} // namespace priv

} // namespace myrmidon

} // namespace fort
