#include "Experiment.hpp"




Experiment::Experiment( QObject * parent)
	: QObject(parent)
	, d_experiment(new fort::myrmidion::priv::Experiment())
	, d_modified(false) {
	if (parent == NULL ) {

	}
}

Experiment::~Experiment() {
}



void Experiment::reset() {
	d_experiment = fort::myrmidion::priv::Experiment::Ptr(new fort::myrmidion::priv::Experiment());
	markModified(false);
}

Error Experiment::open(const QString & path) {
	try {
		d_experiment->Open(path.toUtf8().constData());
		markModified(false);
	} catch( const std::exception & e) {
		return Error(e.what());
	}
	return Error::NONE;
}


Error Experiment::addDataDirectory(const QString & path) {
	d_experiment->d_experiment.add_datadirectory(path.toUtf8().constData());
	markModified(true);
	return Error::NONE;
}

Error Experiment::save(const QString & path ) {
	try {
		d_experiment->Save(path.toUtf8().constData());
		markModified(false);
	} catch (const std::exception & e ) {
		return Error(e.what());
	}
	return Error::NONE;
}


bool Experiment::isModified() const {
	return d_modified;
}

void Experiment::markModified(bool mod) {
	if ( d_modified == mod) {
		return;
	}
	d_modified = mod;
	emit modified(mod);
}
