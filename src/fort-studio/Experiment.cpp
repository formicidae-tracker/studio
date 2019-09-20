#include "Experiment.hpp"




Experiment::Experiment( QObject * parent)
	: QObject(parent)
	, d_modified(false) {
	reset();
}

Experiment::~Experiment() {
}


void Experiment::reset() {
	d_experiment = fort::myrmidion::priv::Experiment::Ptr(new fort::myrmidion::priv::Experiment());
	fort::myrmidion::pb::AntMetadata a;

	a.set_id(1);
	auto idtf = a.add_marker();
	idtf->set_startvalidframe(0);
	auto m = idtf->mutable_marker();
	m->set_id(0);
	m->set_x(1.5);
	m->set_y(-2.0);
	m->set_theta(42.3);

	d_experiment->d_ants.push_back(a);
	a.Clear();
	a.set_id(4236);
	d_experiment->d_ants.push_back(a);
	a.Clear();

	a.set_id(562021);
	idtf = a.add_marker();
	idtf->set_startvalidframe(0);
	m = idtf->mutable_marker();
	m->set_id(234);
	m->set_x(-1.5);
	m->set_y(2.0);
	m->set_theta(-42.3);
	idtf = a.add_marker();
	idtf->set_startvalidframe(19674);
	m = idtf->mutable_marker();
	m->set_id(234);
	m->set_x(2);
	m->set_y(-12.0);
	m->set_theta(89.3);
	idtf = a.add_marker();
	idtf->set_startvalidframe(35894);
	m = idtf->mutable_marker();
	m->set_id(589);
	m->set_x(-5.5);
	m->set_y(-12.0);
	m->set_theta(0.3);


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
