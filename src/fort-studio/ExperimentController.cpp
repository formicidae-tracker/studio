#include "ExperimentController.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>


#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <myrmidon/priv/Identifier.hpp>

namespace fm=fort::myrmidon;
namespace fmp=fm::priv;

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
		                                                             d_experiment->Basedir());


		// d_experiment->AddTrackingDataDirectory(tdd);
		setModified(true);
		emit dataDirUpdated(d_experiment->TrackingDataDirectories());
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}


Error ExperimentController::removeDataDirectory(const QString & path) {
	try {
		// d_experiment->RemoveTrackingDataDirectory(path.toUtf8().constData());
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
		auto exp = fort::myrmidon::priv::Experiment::NewFile(path.toUtf8().constData());
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


fort::myrmidon::priv::Ant::Ptr ExperimentController::createAnt() {
	auto a = d_experiment->Identifier().CreateAnt();
	emit antCreated(a);
	setModified(true);
	return a;
}

Error ExperimentController::addIdentification(fort::myrmidon::Ant::ID ID,
                                              fort::myrmidon::priv::TagID tagValue,
                                              const fm::Time::ConstPtr & start,
                                              const fm::Time::ConstPtr & end) {
	try {
		fmp::Identification::Ptr res = d_experiment->Identifier().AddIdentification(ID,tagValue,start,end);
		emit identificationCreated(res);
		emit antModified(d_experiment->ConstIdentifier().Ants().find(ID)->second);
		setModified(true);
		return Error::NONE;
	} catch (const std::exception & e) {
		return Error(e.what());
	}
}



Error ExperimentController::removeAnt(fort::myrmidon::Ant::ID ID) {
	try {
		auto & identifier = d_experiment->Identifier();
		auto fi = identifier.Ants().find(ID);
		if ( fi == identifier.Ants().end() ) {
			return Error(tr("Unknown Ant %1").arg(fort::myrmidon::priv::Ant::FormatID(ID).c_str()));
		}
		for(const auto & ident : fi->second->Identifications() ) {
			identifier.DeleteIdentification(ident);
			emit identificationDeleted(ident);
			emit antModified(fi->second);
			setModified(true);
		}

		d_experiment->Identifier().DeleteAnt(ID);
		emit antDeleted(fi->second);
		setModified(true);
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}

Error ExperimentController::deleteIdentification(const fort::myrmidon::priv::Identification::Ptr & ident) {
	try {
		auto & identifier = d_experiment->Identifier();
		identifier.DeleteIdentification(ident);
		emit identificationDeleted(ident);
		emit antModified(ident->Target());
		setModified(true);
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
	return Error::NONE;
}


void ExperimentController::setTagFamily(fort::tags::Family tf) {
	if ( tf == d_experiment->Family() ) {
		return;
	}
	try {
		d_experiment->SetFamily(tf);
		setModified(true);
	} catch ( const std::exception & e) {
		qCritical() << e.what();
	}
}

void ExperimentController::setThreshold(uint8_t th) {
	if ( th == d_experiment->Threshold() ) {
		return;
	}
	d_experiment->SetThreshold(th);
	setModified(true);
}
