#include "ExperimentBridge.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

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
	, d_statistics(new StatisticsBridge(this)) {

	connectModifications();


	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        d_measurements,
	        &MeasurementBridge::onTDDAdded);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        d_measurements,
	        &MeasurementBridge::onTDDDeleted);



	connect(d_identifier,
	        &IdentifierBridge::antCreated,
	        d_antMetadata,
	        &AntMetadataBridge::onAntListModified);

	connect(d_identifier,
	        &IdentifierBridge::antDeleted,
	        d_antMetadata,
	        &AntMetadataBridge::onAntListModified);



	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        d_movies,
	        &MovieBridge::onTrackingDataDirectoryAdded);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        d_movies,
	        &MovieBridge::onTrackingDataDirectoryDeleted);

	connect(d_universe,&UniverseBridge::spaceDeleted,
	        d_zones,&ZoneBridge::rebuildSpaces);

	connect(d_universe,&UniverseBridge::spaceAdded,
	        d_zones,&ZoneBridge::rebuildSpaces);

	connect(d_universe,&UniverseBridge::spaceChanged,
	        d_zones,&ZoneBridge::rebuildSpaces);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        d_zones,
	        [this](const fmp::TrackingDataDirectory::Ptr & tdd) {
		        d_zones->onTrackingDataDirectoryChange(tdd->URI().c_str());
	        });
	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        d_zones,&ZoneBridge::onTrackingDataDirectoryChange);

	connect(d_universe,&UniverseBridge::trackingDataDirectoryAdded,
	        d_statistics,&StatisticsBridge::onTrackingDataDirectoryAdded);

	connect(d_universe,&UniverseBridge::trackingDataDirectoryDeleted,
	        d_statistics,&StatisticsBridge::onTrackingDataDirectoryDeleted);

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


bool ExperimentBridge::open(const QString & path) {
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

SelectedIdentificationBridge * ExperimentBridge::selectedIdentification() const {
	return selectedAnt()->selectedIdentification();
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


void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[ExperimentBridge]: setting new fort::myrmidon::priv::Experiment in children";
	d_experiment = experiment;
	d_universe->setExperiment(experiment);
	d_measurements->setExperiment(experiment);
	d_identifier->setExperiment(experiment);
	d_identifier->selectedAnt()->setExperiment(experiment);
	d_identifier->selectedAnt()->selectedIdentification()->setExperiment(experiment);
	d_globalProperties->setExperiment(experiment);
	d_identifiedFrameLoader->setExperiment(experiment);
	d_antShapeTypes->setExperiment(experiment);
	d_antMetadata->setExperiment(experiment);
	d_movies->setExperiment(experiment);
	d_zones->setExperiment(experiment);
	d_statistics->setExperiment(experiment);
	setModified(false);
	resetChildModified();
	emit activated(d_experiment.get() != NULL);
}

void ExperimentBridge::onChildModified(bool modified) {
	qWarning() << "Modified" << modified;
	if ( modified == false ) {
		return;
	}
	setModified(true);
}

void ExperimentBridge::connectModifications() {

	connect(d_universe,&UniverseBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_measurements,&MeasurementBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_identifier,&IdentifierBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_globalProperties,&GlobalPropertyBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_identifier->selectedAnt(),&SelectedAntBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_identifier->selectedAnt()->selectedIdentification(),
	        &SelectedIdentificationBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_antShapeTypes,&AntShapeTypeBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_antMetadata,&AntMetadataBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_movies,&MovieBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_zones,&ZoneBridge::modified,
	        this,&ExperimentBridge::onChildModified);

	connect(d_statistics,&StatisticsBridge::modified,
	        this,&ExperimentBridge::onChildModified);

}

void ExperimentBridge::resetChildModified() {
	d_universe->setModified(false);
	d_measurements->setModified(false);
	d_identifier->setModified(false);
	d_identifier->selectedAnt()->setModified(false);
	d_identifier->selectedAnt()->selectedIdentification()->setModified(false);
	d_globalProperties->setModified(false);
	d_antShapeTypes->setModified(false);
	d_antMetadata->setModified(false);
	d_movies->setModified(false);
	d_zones->setModified(false);
	d_statistics->setModified(false);
}
