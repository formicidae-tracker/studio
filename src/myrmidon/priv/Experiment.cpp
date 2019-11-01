#include "Experiment.hpp"

#include "ExperimentReadWriter.hpp"

#include "Ant.hpp"
#include "Identifier.hpp"

#include "../utils/NotYetImplemented.hpp"

namespace fm = fort::myrmidon;
using namespace fm::priv;

Experiment::Experiment(const fs::path & filepath )
	: d_absoluteFilepath(fs::weakly_canonical(filepath))
	, d_basedir(d_absoluteFilepath.parent_path())
	, d_identifier(Identifier::Create())
	, d_threshold(40)
	, d_family(TagFamily::Unset) {
}

Experiment::Ptr Experiment::Create(const fs::path & filename) {
	return Experiment::Ptr(new Experiment(filename));
}

Experiment::Ptr Experiment::NewFile(const fs::path & filepath) {
	auto absolutePath = fs::weakly_canonical(filepath);
	auto base = absolutePath;
	base.remove_filename();
	auto res = Create(absolutePath);

	fs::create_directories(base);
	res->Save(absolutePath);
	return res;
}


Experiment::Ptr Experiment::Open(const fs::path & filepath) {
	return ExperimentReadWriter::Open(filepath);
}

void Experiment::Save(const fs::path & filepath) const {
	auto basedir = fs::weakly_canonical(d_absoluteFilepath).parent_path();
	auto newBasedir = fs::weakly_canonical(filepath).parent_path();
	//TODO: should not be an error.
	if ( basedir != newBasedir ) {
		throw std::runtime_error("Changing experiment file directory is not yet supported");
	}

	ExperimentReadWriter::Save(*this,filepath);
}

void Experiment::AddTrackingDataDirectory(const TrackingDataDirectory & toAdd) {
	if (d_dataDirs.count(toAdd.Path().generic_string()) != 0 ) {
		throw std::invalid_argument("directory '" + toAdd.Path().string() + "' is already present");
	}

	std::vector<std::string> sortedInTime;
	for(auto const & [path,tdd] : d_dataDirs ) {
		sortedInTime.push_back(path);
	}

	std::sort(sortedInTime.begin(),sortedInTime.end(),
	          [this](const std::string & a, const std::string & b) -> bool{
		          return d_dataDirs[a] < d_dataDirs[b];
	          });

	bool canInsert = d_dataDirs.size() == 0;
	for(auto iter =  sortedInTime.cbegin();
	    iter != sortedInTime.cend();
	    ++iter) {
		auto const & tdd = d_dataDirs[*iter];
		if ( toAdd.EndDate() < tdd.StartDate() ) {
			canInsert = true;
			break;
		}

		if ( tdd.EndDate() >= toAdd.StartDate() ) {
			continue;
		}
		auto next = iter+1;
		if ( next == sortedInTime.cend() ) {
			canInsert = true;
			break;
		}
		auto const & nextTdd = d_dataDirs[*next];

		if ( toAdd.EndDate() < nextTdd.StartDate() ) {
			canInsert = true;
			break;
		}
	}

	if ( canInsert == false ) {
		throw std::runtime_error("Data in '" + toAdd.Path().string() + "' overlaps in time with existing data");
	}

	d_dataDirs[toAdd.Path().generic_string()] = toAdd;
}

bool Experiment::ContainsFramePointer()  const  {
	//check if Identification contains any frame
	for(const auto & [ID,a] :  d_identifier->Ants() ) {
		for (const auto & ident : a->Identifications() ) {
			if ( ident->Start() || ident->End() ) {
				return true;
			}
		}
	}

	// TODO check if other things containing FramePointer contains any..

	return false;
}

void Experiment::RemoveTrackingDataDirectory(fs::path path) {

	if ( path.is_absolute() ) {
		fs::path root = d_absoluteFilepath;
		path = fs::relative(path,root.remove_filename());
	}

	if ( d_dataDirs.count(path.generic_string()) == 0 ) {
		throw std::invalid_argument("Could not find data path '" + path.string() + "'");
	}

	if ( ContainsFramePointer() ) {
		throw std::runtime_error("This Experiment contains FramePointer, and therefore removing a TrackingDataDirectory may breaks everything, and is therefore disabled");
	}

	d_dataDirs.erase(path.generic_string());
}

const Experiment::TrackingDataDirectoryByPath & Experiment::TrackingDataDirectories() const {
	return d_dataDirs;
}

const std::string & Experiment::Name() const {
	return d_name;
}

void Experiment::SetName(const std::string & name) {
	d_name = name;
}

const std::string & Experiment::Author() const {
	return d_author;
}

void Experiment::SetAuthor(const std::string & author) {
	d_author = author;
}

const std::string & Experiment::Comment() const {
	return d_comment;
}

void Experiment::SetComment(const std::string & comment) {
	d_comment = comment;
}

uint8_t Experiment::Threshold() const {
	return d_threshold;
}

void Experiment::SetThreshold(uint8_t th) {
	if ( th == 0 ) {
		th = 1;
	}
	if ( th == 255 ) {
		th = 254;
	}
	d_threshold = th;
}

const fs::path & Experiment::AbsolutePath() const {
	return d_absoluteFilepath;
}

const fs::path & Experiment::Basedir() const {
	return d_basedir;
}


Experiment::TagFamily Experiment::Family() const {
	return d_family;
}

void Experiment::SetFamily(TagFamily tf) {
	d_family = tf;
}


bool Experiment::FreeRangeContaining(Time::ConstPtr & start,
                                     Time::ConstPtr & end,
                                     uint32_t tag, const Time & t) const {
	Time::ConstPtr upperBound, lowerBound;
	try {
		end = d_identifier->UpperUnidentifiedBound(tag,t);
		start = d_identifier->LowerUnidentifiedBound(tag,t);
		return true;
	} catch ( const std::invalid_argument &) {
		end.reset();
		start.reset();
		return false;
	}
}
