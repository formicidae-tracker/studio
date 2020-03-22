#include "Ant.hpp"

#include <sstream>
#include <iomanip>

#include "AntShapeType.hpp"
#include "AntMetadata.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

std::string Ant::FormatID(fort::myrmidon::Ant::ID ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	return os.str();
}

Ant::Ant(const AntShapeTypeContainerConstPtr & shapeTypeContainer,
         const AntMetadataConstPtr & metadata,
         fort::myrmidon::Ant::ID ID)
	: d_ID(ID)
	, d_IDStr(FormatID(ID))
	, d_displayColor(Palette::Default().At(0) )
	, d_displayState(DisplayState::VISIBLE)
	, d_shapeTypes(shapeTypeContainer)
	, d_metadata(metadata) {
	CompileData();
}

Ant::~Ant() {
}

Identification::List & Ant::Accessor::Identifications(Ant & a){
	return a.d_identifications;
}

const Identification::List & Ant::Identifications() const {
	return d_identifications;
}

const Ant::TypedCapsuleList & Ant::Capsules() const {
	return d_capsules;
}

void Ant::DeleteCapsule(size_t index) {
	if ( index >= d_capsules.size() ) {
		throw std::out_of_range("Index "
		                        + std::to_string(index)
		                        + " is out of range [0;"
		                        + std::to_string(d_capsules.size())
		                        + "[");
	}
	d_capsules.erase(d_capsules.begin() + index);
}

void Ant::ClearCapsules() {
	d_capsules.clear();
}

void Ant::AddCapsule(AntShapeTypeID typeID, const CapsulePtr & capsule) {
	if ( d_shapeTypes->Count(typeID) == 0 ) {
		throw std::invalid_argument("Unknown AntShapeTypeID " + std::to_string(typeID));
	}
	if (!capsule) {
		throw std::invalid_argument("No capsule");
	}
	d_capsules.push_back(std::make_pair(typeID,capsule));
}


void Ant::SetDisplayColor(const Color & color) {
	d_displayColor = color;
}

const Color & Ant::DisplayColor() const {
	return d_displayColor;
}

void Ant::SetDisplayStatus(Ant::DisplayState s) {
	d_displayState = s;
}

Ant::DisplayState Ant::DisplayStatus() const {
	return d_displayState;
}

const AntStaticValue & Ant::GetValue(const std::string & name,
                                     const Time & time) {
	return d_compiledData.At(name,time);
}

std::vector<AntTimedValue>::iterator Ant::Find(const AntDataMap::iterator & iter,
                                               const Time::ConstPtr & time) {
	return std::find_if(iter->second.begin(),
	                    iter->second.end(),
	                    [time]( const AntTimedValue & tValue) -> bool {
		                    if ( !time ) {
			                    return !tValue.first;
		                    }
		                    if ( !tValue.first ) {
			                    return false;
		                    }
		                    return time->Equals(*tValue.first);
	                    });
}

bool Ant::CompareTime(const AntTimedValue & a, const AntTimedValue &b) {
	if ( !a.first ) {
		return true;
	}
	if ( !b.first ) {
		return false;
	}
	return a.first->Before(*b.first);
}

void Ant::SetValue(const std::string & name,
                   const AntStaticValue & value,
                   const Time::ConstPtr & time) {
	auto fi = d_metadata->Columns().find(name);
	if ( fi == d_metadata->Columns().end() ) {
		throw std::invalid_argument("Unknown value key '" + name + "'");
	}
	AntMetadata::CheckType(fi->second->MetadataType(),value);
	auto vi = d_data.find(name);
	if ( vi == d_data.end() ) {
		auto res = d_data.insert(std::make_pair(name,std::vector<AntTimedValue>()));
		vi = res.first;
	}
	auto ti = Find(vi,time);
	if ( ti != vi->second.end() ) {
		ti->second = value;
	} else {
		vi->second.push_back(std::make_pair(time,value));
		std::sort(vi->second.begin(),
		          vi->second.end(),
		          &CompareTime);
	}
	CompileData();
}

void Ant::SetValues(const AntDataMap & map) {
	d_data = map;
	for ( auto & [name,tValues] : d_data ) {
		std::sort(tValues.begin(),
		          tValues.end(),
		          &CompareTime);
	}
	CompileData();
}

void Ant::DeleteValue(const std::string & name,
                      const Time::ConstPtr & time) {
	auto vi = d_data.find(name);
	if ( vi == d_data.end() ) {
		throw std::out_of_range("No stored values for '" + name + "'");
	}
	auto ti = Find(vi,time);
	if ( ti == vi->second.end() ) {
		throw std::out_of_range("No stored values for '" + name + "' at requested time");
	}
	vi->second.erase(ti);
	CompileData();
}

const AntDataMap & Ant::DataMap() const {
	return d_data;
}

void Ant::CompileData() {
	std::map<std::string,AntStaticValue> defaults;
	for ( const auto & [name,column] : d_metadata->Columns() ) {
		defaults.insert(std::make_pair(name,AntMetadata::DefaultValue(column->MetadataType())));
	}
	d_compiledData.Clear();

	for ( const auto & [name,tValues] : d_data ) {
		for ( const auto & [time,value] : tValues ) {
			if ( !time ) {
				defaults.erase(name);
			}
			d_compiledData.Insert(name,value,time);
		}
	}

	for ( const auto & [name,defaultValue] : defaults ) {
		d_compiledData.Insert(name,defaultValue,Time::ConstPtr());
	}

}

} // namespace priv
} // namespace myrmidon
} // namespace fort
