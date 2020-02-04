#include "Identifier.hpp"

#include "Ant.hpp"
#include "DeletedReference.hpp"
#include "AntPoseEstimate.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Identifier::UnmanagedAnt::UnmanagedAnt(fort::myrmidon::Ant::ID id) noexcept
	: std::runtime_error([id](){
		                     std::ostringstream os;
		                     os << "Ant:" << Ant::FormatID(id) <<  " is not managed by this object";
		                     return os.str();
	                     }()) {}

Identifier::UnmanagedIdentification::UnmanagedIdentification(const Identification & ident) noexcept
	: std::runtime_error([&ident](){
		                     std::ostringstream os;
		                     os << "Identification:" << ident <<  " is not managed by this object";
		                     return os.str();
	                     }()) {}

Identifier::UnmanagedTag::UnmanagedTag(TagID ID) noexcept
	: std::runtime_error([ID](){
		                     std::ostringstream os;
		                     os << "Tag:" << ID <<  " is not managed by this object";
		                     return os.str();
	                     }()) {}


Identifier::AlreadyExistingAnt::AlreadyExistingAnt(fort::myrmidon::Ant::ID id) noexcept
	: std::runtime_error([id](){
		                     std::ostringstream os;
		                     os << "Ant:" << Ant::FormatID(id) <<  " already exist";
		                     return os.str();
	                     }()) {}

Identifier::Identifier()
	: d_continuous(false) {

}

Identifier::Ptr Identifier::Create() {
    std::shared_ptr<Identifier> res(new Identifier());
    res->d_itself = res;
    return res;
}

Identifier Identifier::Invalid() {
	return Identifier();
}


AntPtr Identifier::CreateAnt(fort::myrmidon::Ant::ID ID ) {
	if ( ID  == NEXT_AVAILABLE_ID ) {
		ID = NextAvailableID();
	}
	if ( d_antIDs.count(ID) != 0 ) {
		throw AlreadyExistingAnt(ID);
	}
	auto res = std::make_shared<Ant>(ID);
	d_ants[res->ID()] =  res;
	d_antIDs.insert(res->ID());
	return res;
}

void Identifier::DeleteAnt(fort::myrmidon::Ant::ID id) {
	auto fi = d_ants.find(id);
	if ( fi == d_ants.end() ) {
		throw UnmanagedAnt(id);
	}
	if ( id != d_ants.size() ) {
		d_continuous = false;
	}


	if ( !fi->second->Identifications().empty() ) {
		std::ostringstream os;
		os <<"Ant:" <<fi->second->FormattedID() << " has Identification, delete them first";
		throw std::logic_error(os.str());
	}

	d_antIDs.erase(id);
	d_ants.erase(id);

}


const AntByID & Identifier::Ants() const {
	return d_ants;
}


fort::myrmidon::Ant::ID Identifier::NextAvailableID() {
	if ( d_continuous == true ) {
		return d_ants.size() + 1;
	}
	fort::myrmidon::Ant::ID res = 0;
	auto missingIndex = std::find_if(d_antIDs.begin(),d_antIDs.end(),[&res] (fort::myrmidon::Ant::ID toTest ) {
		                                                                 return ++res != toTest;
	                                                                 });

	if (missingIndex == d_antIDs.end() ) {
		const_cast<Identifier*>(this)->d_continuous = true;
		return d_antIDs.size() + 1;
	}
	return res;
}

Identifier::Ptr Identifier::Itself() const {
	auto res = d_itself.lock();

	if ( !res ) {
		throw DeletedReference<Identifier>();
	}
	return res;
}


Identification::Ptr Identifier::AddIdentification(fort::myrmidon::Ant::ID id,
                                                  TagID tagValue,
                                                  const Time::ConstPtr & start,
                                                  const Time::ConstPtr & end) {
	if ( d_antIDs.count(id) == 0 ) {
		throw UnmanagedAnt(id);
	}
	auto ant = d_ants[id];

	auto res = Identification::Accessor::Create(tagValue,Itself(),ant);
	Identification::Accessor::SetStart(*res,start);
	Identification::Accessor::SetEnd(*res,end);
	Identification::List current = d_identifications[tagValue];
	current.push_back(res);

	Identification::List antIdents = Ant::Accessor::Identifications(*ant);
	antIdents.push_back(res);

	SortAndCheck(current,antIdents);

	d_identifications[tagValue] = current;
	Ant::Accessor::Identifications(*ant) = antIdents;
	return res;
}

void Identifier::DeleteIdentification(const IdentificationPtr & ident) {
	if ( this != ident->ParentIdentifier().get() ) {
		throw UnmanagedIdentification(*ident);
	}


	auto siblings = d_identifications.find(ident->TagValue());
	if ( siblings == d_identifications.end() ) {
		throw UnmanagedIdentification(*ident);
	}

	auto toErase = siblings->second.begin();
	for( ; toErase != siblings->second.end(); ++toErase ) {
		if ( ident.get() == toErase->get() ) {
			break;
		}
	}

	if ( toErase == siblings->second.end() ) {
		throw UnmanagedIdentification(*ident);
	}

	auto ant = ident->Target();
	if ( d_antIDs.count(ant->ID()) == 0 ) {
		throw UnmanagedAnt(ant->ID());
	}

	auto toEraseAnt = ant->Identifications().begin();
	for(;toEraseAnt != ant->Identifications().end();++toEraseAnt ) {
		if ( ident.get() == toEraseAnt->get() ) {
			break;
		}
	}

	if ( toEraseAnt == ant->Identifications().end() ) {
		throw UnmanagedIdentification(*ident);
	}

	siblings->second.erase(toErase);
	Ant::Accessor::Identifications(*ant).erase(toEraseAnt);

}

Identification::List & Identifier::Accessor::IdentificationsForTag(Identifier & identifier,TagID tagID) {
	auto fi = identifier.d_identifications.find(tagID);
	if ( fi == identifier.d_identifications.end() ) {
		throw UnmanagedTag(tagID);
	}
	return fi->second;
}

void Identifier::SortAndCheck(IdentificationList & tagSiblings,
                              IdentificationList & antSiblings) {
	auto overlap = TimeValid::SortAndCheckOverlap(tagSiblings.begin(),tagSiblings.end());
	if ( overlap.first != overlap.second ) {
		throw OverlappingIdentification(**overlap.first,**overlap.second);
	}

	overlap = TimeValid::SortAndCheckOverlap(antSiblings.begin(),antSiblings.end());
	if ( overlap.first != overlap.second ) {
		throw OverlappingIdentification(**overlap.first,**overlap.second);
	}

}

Identification::Ptr Identifier::Identify(TagID tag,const Time & t) const {
	auto fi = d_identifications.find(tag);
	if ( fi == d_identifications.end()) {
		return Identification::Ptr();
	}

	for(const auto & ident : fi->second ) {
		if (ident->IsValid(t) == true ) {
			return ident;
		}
	}
	return Identification::Ptr();
}


Time::ConstPtr Identifier::UpperUnidentifiedBound(TagID tag, const Time & t) const {
	auto fi = d_identifications.find(tag) ;
	if ( fi == d_identifications.end() ) {
		return Time::ConstPtr();
	}

	return TimeValid::UpperUnvalidBound(t,fi->second.begin(),fi->second.end());
}

Time::ConstPtr Identifier::LowerUnidentifiedBound(TagID tag, const Time & t) const {
	auto fi = d_identifications.find(tag) ;
	if ( fi == d_identifications.end() ) {
		return Time::ConstPtr();
	}

	return TimeValid::LowerUnvalidBound(t,fi->second.begin(),fi->second.end());
}


size_t Identifier::UseCount(TagID tag) const {
	auto fi = d_identifications.find(tag);
	if ( fi == d_identifications.end() ) {
		return 0;
	}
	return fi->second.size();
}

bool Identifier::AntPoseTimeComparator::operator() (const AntPoseEstimateConstPtr & a,
                                                    const AntPoseEstimateConstPtr & b) {
	return a->Reference().Time().Before(b->Reference().Time());
}

bool Identifier::FreeRangeContaining(Time::ConstPtr & start,
                                     Time::ConstPtr & end,
                                     TagID tag, const Time & t) const {
	Time::ConstPtr upperBound, lowerBound;
	try {
		end = UpperUnidentifiedBound(tag,t);
		start = LowerUnidentifiedBound(tag,t);
		return true;
	} catch ( const std::invalid_argument &) {
		end.reset();
		start.reset();
		return false;
	}
}

void Identifier::SetAntPoseEstimate(const AntPoseEstimateConstPtr & ape) {
	auto & APEs = d_tagPoseEstimates[ape->TargetTagID()];
	APEs.erase(ape);
	APEs.insert(ape);

	auto identification = Identify(ape->TargetTagID(),ape->Reference().Time());
	if (!identification) {
		return;
	}
	UpdateIdentification(*identification);
}

void Identifier::UpdateIdentification(Identification & identification) {
	std::vector<AntPoseEstimateConstPtr> matched;
	auto & APEs = d_tagPoseEstimates[identification.TagValue()];
	matched.reserve(APEs.size());
	for (const auto & ape : APEs ) {
		if ( ape->TargetTagID() != identification.TagValue() ) {
			throw std::logic_error("Unexpected TagID");
		}
		if ( identification.IsValid(ape->Reference().Time()) == false ) {
			continue;
		}
		matched.push_back(ape);
	}
	Eigen::Vector2d newPosition;
	double newAngle;
	AntPoseEstimate::ComputeMeanPose(newPosition,newAngle,matched.begin(),matched.end());
	identification.SetAntPosition(newPosition,newAngle);
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
