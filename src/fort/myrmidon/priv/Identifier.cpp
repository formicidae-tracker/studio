#include "Identifier.hpp"

#include "Ant.hpp"
#include "Experiment.hpp"
#include "DeletedReference.hpp"
#include "AntPoseEstimate.hpp"

#include <iostream>

namespace fort {
namespace myrmidon {
namespace priv {

IdentifierIF::~IdentifierIF() {}


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

Identifier::Identifier()
	: d_callback([](const Identification::Ptr &, const std::vector<AntPoseEstimateConstPtr> &){}) {
}

Identifier::~Identifier() {}

AntPtr Identifier::CreateAnt(const AntShapeTypeContainerConstPtr & shapeTypes,
                             const AntMetadataConstPtr & antMetadata,
                             fort::myrmidon::AntID ID ) {
	return CreateObject([&shapeTypes,&antMetadata](fort::myrmidon::AntID ID) { return std::make_shared<Ant>(shapeTypes,antMetadata,ID); },ID);
}

void Identifier::DeleteAnt(fort::myrmidon::AntID ID) {
	auto fi = Ants().find(ID);
	if ( fi != Ants().end() && fi->second->Identifications().empty() == false ) {
		std::ostringstream os;
		os <<"Ant:" <<fi->second->FormattedID() << " has Identification, delete them first";
		throw std::logic_error(os.str());
	}

	DeleteObject(ID);
}


const AntByID & Identifier::Ants() const {
	return Objects();
}




Identification::Ptr Identifier::AddIdentification(const Identifier::Ptr & itself,
                                                  fort::myrmidon::AntID ID,
                                                  TagID tagValue,
                                                  const Time & start,
                                                  const Time & end) {
	auto fi = itself->Ants().find(ID);
	if ( fi == itself->Ants().end() ) {
		throw UnmanagedObject(ID);
	}
	auto ant = fi->second;

	auto res = Identification::Accessor::Create(tagValue,itself,ant);
	Identification::Accessor::SetStart(*res,start);
	Identification::Accessor::SetEnd(*res,end);
	Identification::List current = itself->d_identifications[tagValue];
	current.push_back(res);

	Identification::List antIdents = Ant::Accessor::Identifications(*ant);
	antIdents.push_back(res);

	SortAndCheck(current,antIdents);

	itself->d_identifications[tagValue] = current;
	Ant::Accessor::Identifications(*ant) = antIdents;

	itself->UpdateIdentificationAntPosition(res);

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
	if ( Ants().find(ant->AntID()) == Ants().end() ) {
		throw UnmanagedObject(ant->AntID());
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

void Identifier::Accessor::UpdateIdentificationAntPosition(Identifier & identifier,
                                                           const IdentificationPtr & identification) {
	identifier.UpdateIdentificationAntPosition(identification);
}

void Identifier::Accessor::UpdateIdentificationAntPosition(Identifier & identifier,
                                                           Identification * identificationPtr) {
	auto fi = identifier.d_identifications.find(identificationPtr->TagValue());
	if ( fi == identifier.d_identifications.end() ) {
		return;
	}
	auto ffi = std::find_if(fi->second.begin(),
	                        fi->second.end(),
	                        [identificationPtr](const Identification::Ptr & ident) {
		                        return ident.get() == identificationPtr;
	                        });
	if ( ffi == fi->second.end() ) {
		return;
	}
	identifier.UpdateIdentificationAntPosition(*ffi);
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

Identification::ConstPtr Identifier::Identify(TagID tag,const Time & t) const {
	auto fi = d_identifications.find(tag);
	if ( fi == d_identifications.end()) {
		return Identification::Ptr();
	}

	for( const auto & ident : fi->second ) {
		if (ident->IsValid(t) == true ) {
			return ident;
		}
	}

	return Identification::Ptr();
}

Time Identifier::UpperUnidentifiedBound(TagID tag, const Time & t) const {
	auto fi = d_identifications.find(tag) ;
	if ( fi == d_identifications.end() ) {
		return Time::Forever();
	}

	return TimeValid::UpperUnvalidBound(t,fi->second.begin(),fi->second.end());
}

Time Identifier::LowerUnidentifiedBound(TagID tag, const Time & t) const {
	auto fi = d_identifications.find(tag) ;
	if ( fi == d_identifications.end() ) {
		return Time::SinceEver();
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

bool Identifier::AntPoseEstimateComparator::operator() (const AntPoseEstimateConstPtr & a,
                                                        const AntPoseEstimateConstPtr & b) const {
	return a->URI() < b->URI();
}

bool Identifier::FreeRangeContaining(Time & start,
                                     Time & end,
                                     TagID tag,
                                     const Time & t) const {
	try {
		end = UpperUnidentifiedBound(tag,t);
		start = LowerUnidentifiedBound(tag,t);
		return true;
	} catch ( const std::invalid_argument &) {
		return false;
	}
}

void Identifier::SetAntPoseEstimate(const AntPoseEstimateConstPtr & ape) {
	// create or get existing AntPoseEstimateList
	auto fi = d_tagPoseEstimates.insert(std::make_pair(ape->TargetTagID(),
	                                                   AntPoseEstimateList())).first;
	fi->second.erase(ape);
	fi->second.insert(ape);

	auto identification = std::const_pointer_cast<Identification>(Identify(ape->TargetTagID(),ape->Reference().Time()));
	if (!identification) {
		return;
	}
	UpdateIdentificationAntPosition(identification);
}

void Identifier::DeleteAntPoseEstimate(const AntPoseEstimateConstPtr & ape ) {
	auto fi = d_tagPoseEstimates.find(ape->TargetTagID());
	if ( fi == d_tagPoseEstimates.end() ) {
		return;
	}
	fi->second.erase(ape);
	auto identification = std::const_pointer_cast<Identification>(Identify(ape->TargetTagID(),ape->Reference().Time()));
	if ( !identification ) {
		return;
	}
	UpdateIdentificationAntPosition(identification);
}


void Identifier::QueryAntPoseEstimate(std::vector<AntPoseEstimateConstPtr> & estimations,
                                      const Identification::ConstPtr & identification) const {
	estimations.clear();
	if ( identification->HasUserDefinedAntPose() == true ) {
		return;
	}
	auto APEs = d_tagPoseEstimates.find(identification->TagValue());
	if ( APEs == d_tagPoseEstimates.cend() ) {
		return;
	}
	estimations.reserve(APEs->second.size());
	for (const auto & ape : APEs->second ) {
		if ( ape->TargetTagID() != identification->TagValue() ) {
			throw std::logic_error("Unexpected TagID");
		}
		if ( identification->IsValid(ape->Reference().Time()) == false ) {
			continue;
		}
		estimations.push_back(ape);
	}
}

void Identifier::UpdateIdentificationAntPosition(const Identification::Ptr & identification) {
	if ( identification->HasUserDefinedAntPose() == true ) {
		return;
	}
	std::vector<AntPoseEstimateConstPtr> matched;
	QueryAntPoseEstimate(matched,identification);
	Eigen::Vector2d newPosition;
	double newAngle;
	AntPoseEstimate::ComputeMeanPose(newPosition,newAngle,matched.begin(),matched.end());
	if ( newPosition != identification->AntPosition() || newAngle != identification->AntAngle() ) {
		Identification::Accessor::SetAntPosition(*identification,newPosition,newAngle);
		d_callback(identification,matched);
	}
}

void Identifier::SetAntPositionUpdateCallback(const OnPositionUpdateCallback & callback) {
	d_callback =  callback;
}


Identifier::Compiled::Compiled(const Identifier::IdentificationByTagID & identifications) {
	for ( const auto & [tagID,idents] : identifications ) {
		d_identifications.insert(std::make_pair(tagID+1,idents));
	}
}

Identifier::Compiled::~Compiled() {
}

Identification::ConstPtr Identifier::Compiled::Identify(TagID tagID, const Time & time) const {
	try {
		for ( const auto & i : d_identifications.at(tagID+1) ) {
			if ( i->IsValid(time) == true ) {
					return i;
			}
		}
	} catch ( const std::out_of_range & ) {
		return Identification::Ptr();
	}
	return Identification::Ptr();
}

Identifier::Compiled::ConstPtr Identifier::Compile() const {
	return std::make_shared<Compiled>(d_identifications);
}

std::map<AntID,TagID> Identifier::IdentificationsAt(const Time & time,
                                                    bool removeUnidentifiedAnt) const {
	std::map<AntID,TagID> res;
	for ( const auto & [antID,a] : Ants() ) {
		try {
			auto tagID = a->IdentifiedAt(time);
			res[antID] = tagID;
		} catch ( const std::exception & e) {
			if (removeUnidentifiedAnt == false ) {
				res[antID] = std::numeric_limits<TagID>::max();
			}
		}
	}
	return res;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
