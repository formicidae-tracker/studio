#include "Experiment.hpp"

#include <QFileInfo>

#include <QDebug>

Experiment::Experiment( QObject * parent)
	: QObject(parent)
	, d_modified(false) {
	reset();
}

Experiment::~Experiment() {
}

void Experiment::setPath(const QString & path) {
	QString oldPath = d_absolutePath;
	d_absolutePath = QFileInfo(path).absoluteFilePath();
	if ( oldPath != d_absolutePath) {
		emit pathModified(d_absolutePath);
	}
}

const QString & Experiment::AbsolutePath() const  {
	return d_absolutePath;
}

void Experiment::reset() {
	using namespace fort::myrmidion;
	d_experiment = priv::Experiment::Ptr(new priv::Experiment());
	auto a  = new pb::AntMetadata();

	a->set_id(1);
	auto idtf = a->add_marker();
	idtf->set_startvalidframe(0);
	auto m = idtf->mutable_marker();
	m->set_id(0);
	m->set_x(1.5);
	m->set_y(-2.0);
	m->set_theta(42.3);
	d_experiment->AddAnt(a);

	a = new pb::AntMetadata();
	a->set_id(4236);
	d_experiment->AddAnt(a);

	a = new pb::AntMetadata();
	a->set_id(56201);
	idtf = a->add_marker();
	idtf->set_startvalidframe(0);
	m = idtf->mutable_marker();
	m->set_id(234);
	m->set_x(-1.5);
	m->set_y(2.0);
	m->set_theta(-42.3);
	idtf = a->add_marker();
	idtf->set_startvalidframe(19674);
	m = idtf->mutable_marker();
	m->set_id(234);
	m->set_x(2);
	m->set_y(-12.0);
	m->set_theta(89.3);
	idtf = a->add_marker();
	idtf->set_startvalidframe(35894);
	m = idtf->mutable_marker();
	m->set_id(589);
	m->set_x(-5.5);
	m->set_y(-12.0);
	m->set_theta(0.3);
	d_experiment->AddAnt(a);


	emit antListModified();
	markModified(false);
	setPath("");
}

Error Experiment::open(const QString & path) {
	try {
		d_experiment->Open(path.toUtf8().constData());
		setPath(path);
		markModified(false);
		emit antListModified();
	} catch( const std::exception & e) {
		return Error(e.what());
	}
	return Error::NONE;
}


Error Experiment::openAndParseTrackingDataDirectory(const QString & relativePath, const QString & root,
                                                    fort::myrmidion::pb::TrackingDataDirectory & res) {

	return Error("Not yet implemented");
}


Error Experiment::addDataDirectory(const QString & path) {
	fort::myrmidion::pb::TrackingDataDirectory tdd;
	Error err = openAndParseTrackingDataDirectory(path, QFileInfo(d_absolutePath).absolutePath(), tdd);
	if ( !err.OK() ) {
		return err;
	}
	try {
		d_experiment->AddTrackingDataDirectory(tdd);
		markModified(true);
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}


Error Experiment::removeDataDirectory(const QString & path) {
	try {
		//		d_experiment->RemoveRelativeDataPath(path.toUtf8().constData());
		markModified(true);
		return Error::NONE;
	} catch ( const std::exception & e) {
		return Error(e.what());
	}
}

Error Experiment::save(const QString & path ) {
	try {
		d_experiment->Save(path.toUtf8().constData());
		markModified(false);
		setPath(path);
	} catch (const std::exception & e ) {
		return Error(e.what());
	}
	return Error::NONE;
}

const std::vector<fort::myrmidion::priv::Ant::Ptr> & Experiment::Ants() const {
	return d_experiment->Ants();
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
