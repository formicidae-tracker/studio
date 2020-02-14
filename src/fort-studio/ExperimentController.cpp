#include "ExperimentController.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

namespace fm=fort::myrmidon;
namespace fmp=fm::priv;

ExperimentController::ExperimentController(QObject * parent)
	: QObject(parent)
	, d_modified(false)
	, d_universe(new UniverseBridge(this))
	, d_measurements(new MeasurementBridge(this))
	, d_identifier(new IdentifierBridge(this))
	, d_expBridge(new ExperimentBridge(this))
	, d_selectedAnt(new SelectedAntBridge(this))
	, d_selectedIdentification(new SelectedIdentificationBridge(this)) {

	connectModifications();

	connect(d_identifier,
	        &IdentifierBridge::identificationCreated,
	        d_selectedAnt,
	        &SelectedAntBridge::onIdentificationModified);

	connect(d_identifier,
	        &IdentifierBridge::identificationDeleted,
	        d_selectedAnt,
	        &SelectedAntBridge::onIdentificationModified);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::identificationModified,
	        d_selectedAnt,
	        &SelectedAntBridge::onIdentificationModified);

	connect(d_expBridge,
	        &ExperimentBridge::detectionSettingChanged,
	        d_measurements,
	        &MeasurementBridge::onDetectionSettingChanged);
}

bool ExperimentController::isActive() const {
	return d_experiment.get() != NULL;
}

const fs::path & ExperimentController::absoluteFilePath() const {
	if ( !d_experiment ) {
		static fs::path empty;
		return empty;
	}
	return d_experiment->AbsoluteFilePath();
}


bool ExperimentController::save() {
	if ( !d_experiment ) {
		return false;
	}
	return saveAs(d_experiment->AbsoluteFilePath().c_str());
}


bool ExperimentController::saveAs(const QString & path ) {
	try {
		d_experiment->Save(path.toUtf8().constData());
		setModified(false);
	} catch (const std::exception & e ) {
		qWarning() << "Could not save experiment to '"
		           << path << "': " << e.what();
		return false;
	}
	return true;
}

bool ExperimentController::isModified() const {
	return d_modified;
}

void ExperimentController::setModified(bool mod) {
	if ( d_modified == mod) {
		return;
	}
	d_modified = mod;
	emit modified(mod);
}


bool ExperimentController::open(const QString & path) {
	fmp::Experiment::Ptr experiment;
	try {
		experiment = fmp::Experiment::Open(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qWarning() << "Could not open '" << path
		           << "': " << e.what();
		return false;
	}
	setExperiment(experiment);
	return true;
}


bool ExperimentController::create(const QString & path) {
	fmp::Experiment::Ptr experiment;
	try {
		experiment = fmp::Experiment::NewFile(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qWarning() << "Could not create file '" << path
		           << "': " << e.what();
		return false;
	}
	setExperiment(experiment);
	return true;
}


UniverseBridge * ExperimentController::universe() {
	return d_universe;
}

MeasurementBridge * ExperimentController::measurements() {
	return d_measurements;
}

IdentifierBridge * ExperimentController::identfier() {
	return d_identifier;
}

ExperimentBridge * ExperimentController::experiment() {
	return d_expBridge;
}

SelectedAntBridge * ExperimentController::selectedAnt() {
	return d_selectedAnt;
}

SelectedIdentificationBridge * ExperimentController::selectedIdentification() {
	return d_selectedIdentification;
}

void ExperimentController::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
	d_universe->setExperiment(experiment);
	d_measurements->setExperiment(experiment);
	d_identifier->setExperiment(experiment);
	d_expBridge->setExperiment(experiment);
	d_selectedAnt->setAnt(fmp::Ant::Ptr());
	d_selectedIdentification->setIdentification(fmp::Identification::Ptr());
	emit activated(d_experiment.get() != NULL);
}

void ExperimentController::setModifiedTrue() {
	setModified(true);
}


void ExperimentController::connectModifications() {
	connect(d_universe,
	        &UniverseBridge::spaceDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_universe,
	        &UniverseBridge::spaceAdded,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_universe,
	        &UniverseBridge::spaceChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_measurements,
	        &MeasurementBridge::measurementModified,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_measurements,
	        &MeasurementBridge::measurementDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_measurements,
	        &MeasurementBridge::measurementTypeModified,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_measurements,
	        &MeasurementBridge::measurementTypeDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_identifier,
	        &IdentifierBridge::antCreated,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_identifier,
	        &IdentifierBridge::antDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_identifier,
	        &IdentifierBridge::identificationCreated,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_identifier,
	        &IdentifierBridge::identificationDeleted,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_identifier,
	        &IdentifierBridge::antDisplayChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::nameChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::authorChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::commentChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::tagFamilyChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::thresholdChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_expBridge,
	        &ExperimentBridge::tagSizeChanged,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::startModified,
	        this,
	        &ExperimentController::setModifiedTrue);

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::endModified,
	        this,
	        &ExperimentController::setModifiedTrue);
}
