#include "Identifier.hpp"

#include "Ant.hpp"
#include "DeletedReference.hpp"
#include "AntPoseEstimate.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


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
	: d_callback([](const Identification::Ptr &){}) {
}

Identifier::~Identifier() {}

Identifier::Ptr Identifier::Create() {
    std::shared_ptr<Identifier> res(new Identifier());
    res->d_itself = res;
    return res;
}

Identifier Identifier::Invalid() {
	return Identifier();
}


AntPtr Identifier::CreateAnt(fort::myrmidon::Ant::ID ID ) {
	return CreateObject([](fort::myrmidon::Ant::ID ID) { return std::make_shared<Ant>(ID); },ID);
}

void Identifier::DeleteAnt(fort::myrmidon::Ant::ID ID) {
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


Identifier::Ptr Identifier::Itself() const {
	auto res = d_itself.lock();

	if ( !res ) {
		throw DeletedReference<Identifier>();
	}
	return res;
}


Identification::Ptr Identifier::AddIdentification(fort::myrmidon::Ant::ID ID,
                                                  TagID tagValue,
                                                  const Time::ConstPtr & start,
                                                  const Time::ConstPtr & end) {
	auto fi = Ants().find(ID);
	if ( fi == Ants().end() ) {
		throw UnmanagedObject(ID);
	}
	auto ant = fi->second;

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

	UpdateIdentificationAntPosition(res);

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
	if ( Ants().find(ant->ID()) == Ants().end() ) {
		throw UnmanagedObject(ant->ID());
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

Identification::Ptr Identifier::Identify(TagID tag,const Time & t) const {
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

bool Identifier::AntPoseEstimateComparator::operator() (const AntPoseEstimateConstPtr & a,
                                                        const AntPoseEstimateConstPtr & b) const {
	return a->URI() < b->URI();
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
	// create or get existing AntPoseEstimateList
	auto fi = d_tagPoseEstimates.insert(std::make_pair(ape->TargetTagID(),
	                                                   AntPoseEstimateList())).first;
	fi->second.erase(ape);
	fi->second.insert(ape);

	auto identification = Identify(ape->TargetTagID(),ape->Reference().Time());
	if (!identification) {
		return;
	}
	UpdateIdentificationAntPosition(identification);
}



void Identifier::UpdateIdentificationAntPosition(const Identification::Ptr & identification) {
	std::vector<AntPoseEstimateConstPtr> matched;
	auto & APEs = d_tagPoseEstimates[identification->TagValue()];
	matched.reserve(APEs.size());
	for (const auto & ape : APEs ) {
		if ( ape->TargetTagID() != identification->TagValue() ) {
			throw std::logic_error("Unexpected TagID");
		}
		if ( identification->IsValid(ape->Reference().Time()) == false ) {
			continue;
		}
		matched.push_back(ape);
	}
	Eigen::Vector2d newPosition;
	double newAngle;
	AntPoseEstimate::ComputeMeanPose(newPosition,newAngle,matched.begin(),matched.end());
	if ( newPosition != identification->AntPosition() || newAngle != identification->AntAngle() ) {
		identification->SetAntPosition(newPosition,newAngle);
		d_callback(identification);
	}
}

void Identifier::SetAntPositionUpdateCallback(const OnPositionUpdateCallback & callback) {
	d_callback =  callback;
}


Identifier::Compiled::Compiled(const Identifier::IdentificationByTagID & identifications) {
	Build(identifications);
}

Identifier::Compiled::~Compiled() {
}

Identification::Ptr Identifier::Compiled::Identify(TagID tagID, const Time & time) const {
	if ( d_identifications.empty() == true || time.Before(d_identifications.begin()->first) == true ) {
		return IdentifyFromMap(d_firstIdentifications,tagID);
	}
	auto fi = d_identifications.upper_bound(time);
	if ( fi == d_identifications.end() ) {
		return IdentifyFromMap(d_lastIdentifications,tagID);
	}
	return IdentifyFromMap(fi->second,tagID);
}

void Identifier::Compiled::Build(const Identifier::IdentificationByTagID & identifications) {
	std::set<Time,Time::Comparator> times;
	for ( const auto & [tagID,idents] : identifications ) {
		for ( const auto & i : idents) {
			if ( i->Start() ) {
				times.insert(*(i->Start()));
			}
			if ( i->End() ) {
				times.insert(*(i->End()));
			}
		}
	}
	Time time;
	if ( !times.empty() ) {
		time = times.begin()->Add(-1 * Duration::Nanosecond);
	}
	d_firstIdentifications = BuildMapAtTime(identifications,time);
	for ( const auto & t : times ) {
		d_identifications.insert(std::make_pair(t,BuildMapAtTime(identifications,t)));
	}
	if ( times.empty() ) {
		return;
	}
	time = (--times.end())->Add(1 * Duration::Nanosecond);
	d_lastIdentifications = BuildMapAtTime(identifications,time);
}


Identifier::Compiled::IdentificationsByTagID
Identifier::Compiled::BuildMapAtTime(const Identifier::IdentificationByTagID & identifications,
                                     const Time & time) const {
	Identifier::Compiled::IdentificationsByTagID results;

	for ( const auto & [tagID,idents] : identifications ) {
		for ( const auto & i : idents ) {
			if ( i->IsValid(time) == true ) {
				results.insert(std::make_pair(tagID+1,i));
				break;
			}
		}
	}
}

const Identification::Ptr &
Identifier::Compiled::IdentifyFromMap(const Compiled::IdentificationsByTagID & identifications,
                                      TagID tagID) const {
	static Identification::Ptr empty;
	if ( identifications.count(tagID+1) == 0 ) {
		return empty;
	}
	return identifications.at(tagID+1);
}

Identifier::Compiled::ConstPtr Identifier::Compile() const {
	return std::make_shared<Compiled>(d_identifications);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
