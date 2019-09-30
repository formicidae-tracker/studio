#include "ExperimentController.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>

namespace fmp=fort::myrmidon::priv;

ExperimentController::ExperimentController(fmp::Experiment::Ptr & experiment,
                                           QObject * parent)
	: QObject(parent)
	, d_modified(false)
	, d_experiment(std::move(experiment)) {

}

ExperimentController::~ExperimentController() {
}

Error ExperimentController::addDataDirectory(const QString & path) {

	try {
		auto tdd = fort::myrmidon::priv::TrackingDataDirectory::Open(path.toUtf8().constData(),
		                                                             d_experiment->AbsolutePath().remove_filename());


		d_experiment->AddTrackingDataDirectory(tdd);
		setModified(true);
		emit dataDirUpdated(d_experiment->TrackingDataDirectories());
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}


Error ExperimentController::removeDataDirectory(const QString & path) {
	try {
		d_experiment->RemoveTrackingDataDirectory(path.toUtf8().constData());
		setModified(true);
		emit dataDirUpdated(d_experiment->TrackingDataDirectories());
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}

Error ExperimentController::save(const QString & path ) {
	try {
		d_experiment->Save(path.toUtf8().constData());
		setModified(false);
	} catch (const std::exception & e ) {
		return Error(e.what());
	}
	return Error::NONE;
}

const fort::myrmidon::priv::Experiment & ExperimentController::experiment() const {
	return *d_experiment;
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


ExperimentController * ExperimentController::open(const QString & path, QObject * parent, Error & error) {
	try {
		auto exp = fort::myrmidon::priv::Experiment::Open(path.toUtf8().constData());
		error = Error::NONE;
		return new ExperimentController(exp,parent);
	} catch ( const std::exception & e ) {
		error = Error(tr("Could not open '%1')").arg(path) + e.what());
		return NULL;
	}
}


ExperimentController * ExperimentController::create(const QString & path, QObject * parent, Error & error) {
	try {
		auto exp = fort::myrmidon::priv::Experiment::Create(path.toUtf8().constData());
		error = Error::NONE;
		return new ExperimentController(exp,parent);
	} catch ( const std::exception & e ) {
		error = Error(tr("Could not open '%1')").arg(path) + e.what());
		return NULL;
	}
}


#define implement_setter(Setter) void ExperimentController::set ## Setter( const QString & Setter ## In ) { \
		std::string Setter ## Str = Setter ## In.toUtf8().constData(); \
		if ( Setter ## Str == d_experiment->Setter() ) { \
			return; \
		} \
		d_experiment->Set ## Setter( Setter ## Str ); \
		setModified(true); \
	}

implement_setter(Name)
implement_setter(Author)
implement_setter(Comment)


void ExperimentController::createAnt() {
	d_experiment->CreateAnt();
	emit antListModified(d_experiment->Ants());
	setModified(true);
}


Error ExperimentController::removeAnt(fort::myrmidon::Ant::ID ID) {
	try {
		d_experiment->DeleteAnt(ID);
		emit antListModified(d_experiment->Ants());
		setModified(true);
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}
