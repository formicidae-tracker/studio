#pragma once

#include <memory>
#include <unordered_map>
#include <set>

#include "../Ant.hpp"

namespace fort {

namespace myrmidon {

namespace pb {
class AntMetadata;
}

namespace priv {

class Ant;
typedef std::shared_ptr<Ant> AntPtr;

typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

class Identification;
typedef std::shared_ptr<Identification> IdentificationPtr;
typedef std::vector<IdentificationPtr> IdentificationList;

class FramePointer;
typedef std::shared_ptr<FramePointer> FramePointerPtr;

class Identifier {
public:
	typedef std::shared_ptr<Identifier> Ptr;

	static Ptr Create();

	Ptr Itself() const;

	const static fort::myrmidon::Ant::ID NEXT_AVAILABLE_ID = 0;

	AntPtr CreateAnt(fort::myrmidon::Ant::ID ID = NEXT_AVAILABLE_ID);
	void DeleteAnt(fort::myrmidon::Ant::ID );
	const AntByID & Ants() const;


	IdentificationPtr AddIdentification(fort::myrmidon::Ant::ID id,
	                                    uint32_t tagValue,
	                                    const FramePointerPtr & start,
	                                    const FramePointerPtr & end);

	void DeleteIdentification(const IdentificationPtr & ident);



	class UnmanagedAnt : public std::runtime_error {
	public:
		UnmanagedAnt(fort::myrmidon::Ant::ID id) noexcept;
		virtual ~UnmanagedAnt() noexcept {};
	};

	class UnmanagedTag : public std::runtime_error {
	public:
		UnmanagedTag(uint32_t tagValue) noexcept;
		virtual ~UnmanagedTag() noexcept {};
	};

	class UnmanagedIdentification : public std::runtime_error {
	public:
		UnmanagedIdentification(const Identification & ident) noexcept;
		virtual ~UnmanagedIdentification() noexcept {};
	};

	class AlreadyExistingAnt : public std::runtime_error {
	public:
		AlreadyExistingAnt(fort::myrmidon::Ant::ID id) noexcept;
		virtual ~AlreadyExistingAnt() noexcept {};
	};

	class Accessor {
	private:
		static IdentificationList & IdentificationsForTag(Identifier & identifier,uint32_t tagID);
	public:
		friend class Identification;
	};

	static void SortAndCheck(IdentificationList & tagSibling,
	                         IdentificationList & antSibling);

private:
	typedef std::set<fort::myrmidon::Ant::ID> SetOfID;
	typedef std::unordered_map<uint32_t,IdentificationList> IdentificationByTagID;


	Identifier();

	fort::myrmidon::Ant::ID NextAvailableID();


	std::weak_ptr<Identifier> d_itself;

	AntByID d_ants;
	SetOfID d_antIDs;
	bool    d_continuous;

	IdentificationByTagID d_identifications;
};



} // namespace priv

} // namespace myrmidon

} // namespace fort
