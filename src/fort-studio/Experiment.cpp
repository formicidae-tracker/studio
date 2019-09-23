#include "Experiment.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>

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
	using namespace fort::myrmidon;
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
	emit dataDirUpdated(QStringList());
}

Error Experiment::open(const QString & path) {
	try {
		d_experiment = fort::myrmidon::priv::Experiment::Open(path.toUtf8().constData());
		setPath(path);
		markModified(false);
		emit antListModified();
		QStringList res;
		for ( auto const & p : d_experiment->TrackingDataPath() ) {
			res << p.c_str();
		}
		emit dataDirUpdated(res);
	} catch( const std::exception & e) {
		return Error(e.what());
	}
	return Error::NONE;
}


Error Experiment::openAndParseTrackingDataDirectory(const QString & path, const QString & root,
                                                    fort::myrmidon::pb::TrackingDataDirectory & res) {



	QDir rootDir(root);
	QString toSave = rootDir.relativeFilePath(path);

	//list data

	std::vector<QString> hermesFiles;
	for ( auto const & f : QDir(path).entryInfoList(QStringList() << "*.hermes", QDir::Files) ) {
		hermesFiles.push_back(f.canonicalFilePath());
	}

	std::sort(hermesFiles.begin(),hermesFiles.end());

	if ( hermesFiles.empty() ) {
		return Error("'" + path + "' does not contains any .hermes files");
	}
	uint64_t firstFrame;
	uint64_t lastFrame;
	fort::hermes::FrameReadout ro;
	try {
		fort::hermes::FileContext first(hermesFiles.front().toUtf8().constData());
		first.Read(&ro);
		firstFrame = ro.frameid();
	} catch (const std::exception & e ) {
		return Error("could not read first frame of '" + hermesFiles.front() + "':" + e.what());
	}

	try {
		fort::hermes::FileContext last(hermesFiles.back().toUtf8().constData());
		for (;;) {
			last.Read(&ro);
			lastFrame = ro.frameid();
		}
	} catch (const fort::hermes::EndOfFile & e ) {
	} catch ( const std::exception & e) {
		return Error("could not read last frame of '" + hermesFiles.back() + "':" + e.what());
	}
	res.set_path(toSave.toUtf8().constData());
	res.set_startframe(firstFrame);
	res.set_endframe(lastFrame);

	qInfo() << res.DebugString().c_str();

	return Error::NONE;
}


Error Experiment::addDataDirectory(const QString & path, QString & result) {
	fort::myrmidon::pb::TrackingDataDirectory tdd;
	Error err = openAndParseTrackingDataDirectory(path, QFileInfo(d_absolutePath).absolutePath(), tdd);
	if ( !err.OK() ) {
		return err;
	}
	result = tdd.path().c_str();
	try {
		d_experiment->AddTrackingDataDirectory(tdd);
		markModified(true);
		QStringList res;
		for ( auto const & p : d_experiment->TrackingDataPath() ) {
			res << p.c_str();
		}
		emit dataDirUpdated(res);
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

const std::vector<fort::myrmidon::priv::Ant::Ptr> & Experiment::Ants() const {
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
