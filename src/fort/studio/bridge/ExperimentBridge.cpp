#include "ExperimentBridge.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include <fort/studio/widget/TrackingDataDirectoryLoader.hpp>

#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "GlobalPropertyBridge.hpp"
#include "IdentifierBridge.hpp"
#include "SelectedAntBridge.hpp"
#include "IdentifiedFrameConcurrentLoader.hpp"
#include "AntShapeTypeBridge.hpp"
#include "AntMetadataBridge.hpp"
#include "MovieBridge.hpp"
#include "ZoneBridge.hpp"
#include "StatisticsBridge.hpp"
#include "TagCloseUpBridge.hpp"

namespace fm=fort::myrmidon;
namespace fmp=fm::priv;

ExperimentBridge::~ExperimentBridge() {}

ExperimentBridge::ExperimentBridge(QObject * parent)
	: Bridge(parent)
	, d_universe(new UniverseBridge(this))
	, d_measurements(new MeasurementBridge(this))
	, d_identifier(new IdentifierBridge(this))
	, d_globalProperties(new GlobalPropertyBridge(this))
	, d_identifiedFrameLoader(new IdentifiedFrameConcurrentLoader(this))
	, d_antShapeTypes(new AntShapeTypeBridge(this))
	, d_antMetadata(new AntMetadataBridge(this))
	, d_movies(new MovieBridge(this))
	, d_zones(new ZoneBridge(this))
	, d_statistics(new StatisticsBridge(this))
	, d_tagCloseUps(new TagCloseUpBridge(this))
	, d_children({
	              d_universe,
	              d_measurements,
	              d_identifier,
	              d_globalProperties,
	              d_antShapeTypes,
	              d_antMetadata,
	              d_movies,
	              d_zones,
	              d_statistics,
		 d_tagCloseUps,
		}) {


	for ( const auto & child : d_children ) {
		connect(child,&Bridge::modified,
		        this,&ExperimentBridge::onChildModified);
		child->initialize(this);
	}

	connect(d_identifier->selectedAnt(),&Bridge::modified,
	        this,&ExperimentBridge::onChildModified);


}


bool ExperimentBridge::isActive() const {
	return d_experiment.get() != NULL;
}

const fs::path & ExperimentBridge::absoluteFilePath() const {
	if ( !d_experiment ) {
		static fs::path empty;
		return empty;
	}
	return d_experiment->AbsoluteFilePath();
}


bool ExperimentBridge::save() {
	if ( !d_experiment ) {
		return false;
	}
	return saveAs(d_experiment->AbsoluteFilePath().c_str());
}


bool ExperimentBridge::saveAs(const QString & path ) {
	if ( !d_experiment ) {
		qDebug() << "[ExperimentBridge]: Ignoring ExperimentBridge::saveAs(): no experience loaded";
		return false;
	}
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::Save('" << path << "')";
		d_experiment->Save(path.toUtf8().constData());
		setModified(false);
		resetChildModified();
		qInfo() << "Saved experiment to '" << path << "'";
	} catch (const std::exception & e ) {
		qCritical() << "Could not save experiment to '"
		            << path << "': " << e.what();
		return false;
	}
	return true;
}


bool ExperimentBridge::open(const QString & path,QWidget * parent) {
	fmp::Experiment::Ptr experiment;
	if ( !d_experiment == false
	     && d_experiment->AbsoluteFilePath().c_str() == path ) {
		d_experiment->UnlockFile();
	}
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::Open('" << path << "')";
		experiment = fmp::Experiment::Open(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open '" << path
		            << "': " << e.what();
		return false;
	}

	try {
		std::vector<fmp::TrackingDataDirectory::Ptr> tdds;
		for(const auto & [tddURI,tdd] : experiment->TrackingDataDirectories() ) {
			tdds.push_back(tdd);
		}
		TrackingDataDirectoryLoader::EnsureLoaded(tdds,parent);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open '"
		            << path
		            << "': could not load computed data: "
		            << e.what();
		return false;
	}

	qInfo() << "Opened experiment file '" << path << "'";
	setExperiment(experiment);
	return true;
}


bool ExperimentBridge::create(const QString & path) {
	fmp::Experiment::Ptr experiment;
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::NewFile('" << path << "')";
		experiment = fmp::Experiment::NewFile(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not create file '" << path
		            << "': " << e.what();
		return false;
	}
	qInfo() << "Created new experiment file '" << path  << "'";
	setExperiment(experiment);
	return true;
}


UniverseBridge * ExperimentBridge::universe() const {
	return d_universe;
}

MeasurementBridge * ExperimentBridge::measurements() const {
	return d_measurements;
}

IdentifierBridge * ExperimentBridge::identifier() const {
	return d_identifier;
}

GlobalPropertyBridge * ExperimentBridge::globalProperties() const {
	return d_globalProperties;
}

SelectedAntBridge * ExperimentBridge::selectedAnt() const {
	return d_identifier->selectedAnt();
}


IdentifiedFrameConcurrentLoader * ExperimentBridge::identifiedFrameLoader() const {
	return d_identifiedFrameLoader;
}

AntShapeTypeBridge *  ExperimentBridge::antShapeTypes() const {
	return d_antShapeTypes;
}

AntMetadataBridge *  ExperimentBridge::antMetadata() const {
	return d_antMetadata;
}

MovieBridge *  ExperimentBridge::movies() const {
	return d_movies;
}

ZoneBridge * ExperimentBridge::zones() const {
	return d_zones;
}

StatisticsBridge * ExperimentBridge::statistics() const {
	return d_statistics;
}

TagCloseUpBridge * ExperimentBridge::tagCloseUps() const {
	return d_tagCloseUps;
}

void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[ExperimentBridge]: setting new fort::myrmidon::priv::Experiment in children";
	d_experiment = experiment;

	for ( const auto & child : d_children ) {
		child->setExperiment(experiment);
	}
	d_identifier->selectedAnt()->setExperiment(experiment);
	d_identifiedFrameLoader->setExperiment(experiment);
	resetChildModified();

	emit activated(d_experiment.get() != NULL);
}

void ExperimentBridge::onChildModified(bool modified) {
	if ( modified == false ) {
		return;
	}
	setModified(true);
}

void ExperimentBridge::resetChildModified() {
	for ( const auto & child : d_children ) {
		child->setModified(false);
	}
	d_identifier->selectedAnt()->setModified(false);
}
