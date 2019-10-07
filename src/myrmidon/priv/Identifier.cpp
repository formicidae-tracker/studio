#include "Identifier.hpp"

#include "Ant.hpp"
#include "DeletedReference.hpp"


using namespace fort::myrmidon::priv;

Identifier::UnmanagedAnt::UnmanagedAnt(fort::myrmidon::Ant::ID id) noexcept
	: std::runtime_error([id](){
		                     std::ostringstream os;
		                     os << "Ant:" << Ant::FormatID(id) <<  " is not managed by this object";
		                     return os.str();
	                     }()) {}

Identifier::UnmanagedIdentification::UnmanagedIdentification(const Identification & ident) noexcept
	: std::runtime_error([ident](){
		                     std::ostringstream os;
		                     os << "Identification:" << ident <<  " is not managed by this object";
		                     return os.str();
	                     }()) {}

Identifier::UnmanagedTag::UnmanagedTag(uint32_t ID) noexcept
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
	if ( d_ants.count(id) == 0 ) {
		throw UnmanagedAnt(id);
	}
	if ( id != d_ants.size() ) {
		d_continuous = false;
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
                                                  uint32_t tagValue,
                                                  const FramePointerPtr & start,
                                                  const FramePointerPtr & end) {
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

Identification::List & Identifier::Accessor::IdentificationsForTag(Identifier & identifier,uint32_t tagID) {
	auto fi = identifier.d_identifications.find(tagID);
	if ( fi == identifier.d_identifications.end() ) {
		throw UnmanagedTag(tagID);
	}
	return fi->second;
}

void Identifier::SortAndCheck(IdentificationList & tagSiblings,
                              IdentificationList & antSiblings) {
	auto overlap = Identification::SortAndCheckOverlap(tagSiblings.begin(),tagSiblings.end());
	if ( overlap.first != overlap.second ) {
		throw OverlappingIdentification(**overlap.first,**overlap.second);
	}

	overlap = Identification::SortAndCheckOverlap(antSiblings.begin(),antSiblings.end());
	if ( overlap.first != overlap.second ) {
		throw OverlappingIdentification(**overlap.first,**overlap.second);
	}

}

Identification::Ptr Identifier::Identify(uint32_t tag,const FramePointer & frame) const {
	auto fi = d_identifications.find(tag);
	if ( fi == d_identifications.end()) {
		return Identification::Ptr();
	}

	for(const auto & ident : fi->second ) {
		if (ident->TargetsFrame(frame) == true ) {
			return ident;
		}
	}
	return Identification::Ptr();
}
