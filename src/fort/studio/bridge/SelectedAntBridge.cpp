
#include "SelectedAntBridge.hpp"

#include "SelectedIdentificationBridge.hpp"

#include <QDebug>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Capsule.hpp>

#include <fort/studio/Format.hpp>

#include "IdentifierBridge.hpp"

SelectedAntBridge::SelectedAntBridge(QObject * parent)
	: Bridge(parent) {

}

SelectedAntBridge::~SelectedAntBridge() {}

bool SelectedAntBridge::isActive() const {
	return d_ant.get() != NULL;
}


void SelectedAntBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
}


const fmp::Ant::TypedCapsuleList & SelectedAntBridge::capsules() const {
	if ( !d_ant ) {
		static fmp::Ant::TypedCapsuleList empty;
		return empty;
	}
	return d_ant->Capsules();
}


void SelectedAntBridge::setAnt(const fmp::Ant::Ptr & ant) {
	setModified(false);
	d_ant = ant;
	if ( !d_ant ) {
		emit activated(false);
		return;
	}
	emit activated(true);
}

fmp::AntID SelectedAntBridge::selectedID() const {
	if ( !d_ant ) {
		return 0;
	}
	return d_ant->AntID();
}


void SelectedAntBridge::addCapsule(fmp::AntShapeTypeID typeID,const fmp::CapsulePtr & capsule) {
	if ( !d_ant || !d_experiment || !capsule) {
		return;
	}
	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Experiment::AddCapsuleToAnt("
		         << d_ant->FormattedID().c_str()
		         << "," << typeID
		         << "," << ToQString(*capsule)
		         << ")";
		d_ant->AddCapsule(typeID,*capsule);
	} catch (const std::exception & e ) {
		qCritical() << "Could not add Capsule of type " << typeID
		            << " to Ant " << d_ant->FormattedID().c_str()
		            << ": " << e.what();
		return;
	}
	setModified(true);
}

void SelectedAntBridge::clearCapsules() {
	if ( !d_ant ) {
		return;
	}

	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Ant("
		         << d_ant->FormattedID().c_str()
		         << ")::DeleteCapsules()";
		d_ant->ClearCapsules();
	} catch ( const std::exception & e ) {
		qCritical() << "Could not remove Capsules "
		            << " from Ant " << d_ant->FormattedID().c_str()
		            << ": " << e.what();
		return;
	}

	setModified(true);
}

std::vector<fmp::Identification::ConstPtr> SelectedAntBridge::identifications() const {
	if ( !d_ant ) {
		return {};
	}
	std::vector<fmp::Identification::ConstPtr> res;
	res.reserve(d_ant->Identifications().size());
	for ( const auto & i : d_ant->Identifications() ) {
		res.push_back(i);
	}
	return res;
}


void SelectedAntBridge::cloneShape(bool scaleToSize, bool overwriteShape) {
	if ( !d_ant == true || !d_experiment == true ) {
		return;
	}

	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Experiment::CloneAntShape("
		         << d_ant->FormattedID().c_str() << ","
		         << scaleToSize << ","
		         << overwriteShape << ")";
		d_experiment->CloneAntShape(d_ant->AntID(),
		                            scaleToSize,
		                            overwriteShape);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not clone Ant shape " << e.what();
		return;
	}
	qInfo() << "Cloned Ant's " << d_ant->FormattedID().c_str() << " Shape.";
	setModified(true);

}
