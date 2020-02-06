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



void ExperimentController::setName(const QString & name) {
	if ( this->name() == name ) {
		return;
	}

	d_experiment->SetName(name.toUtf8().data());
	setModified(true);
	emit nameChanged(this->name());
}

QString ExperimentController::name() const {
	return d_experiment->Name().c_str();
}

void ExperimentController::setAuthor(const QString & author) {
	if ( this->author() == author ) {
		return;
	}
	d_experiment->SetAuthor(author.toUtf8().data());
	setModified(true);
	emit authorChanged(this->author());
}

QString ExperimentController::author() const {
	return d_experiment->Author().c_str();
}

void ExperimentController::setComment(const QString & comment) {
	if( this->comment() == comment ) {
		return;
	}
	d_experiment->SetComment(comment.toUtf8().data());
	setModified(true);
	emit commentChanged(this->comment());
}

QString ExperimentController::comment() const {
	return d_experiment->Comment().c_str();
}

void ExperimentController::setTagFamily(fort::tags::Family tf) {
	if ( d_experiment->Family() == tf ) {
		return;
	}

	d_experiment->SetFamily(tf);
	setModified(true);
	emit tagFamilyChanged(tf);
}

fort::tags::Family ExperimentController::tagFamily() const {
	return d_experiment->Family();
}

void ExperimentController::setThreshold(uint8_t th) {
	if ( d_experiment->Threshold() == th ) {
		return;
	}

	d_experiment->SetThreshold(th);
	setModified(true);
	emit thresholdChanged(this->threshold());
}

uint8_t ExperimentController::threshold() const {
	return d_experiment->Threshold();
}

double ExperimentController::tagSize() const {
	return d_experiment->DefaultTagSize();
}

void ExperimentController::setTagSize(double tagSize) {
	if ( d_experiment->DefaultTagSize() == tagSize ) {
		return;
	}

	d_experiment->SetDefaultTagSize(tagSize);
	setModified(true);
	emit tagSizeChanged(tagSize);
}
