#include "SelectedIdentificationBridge.hpp"

#include <QDebug>


#include <fort/studio/Format.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identification.hpp>

#include <fort/studio/Utils.hpp>

SelectedIdentificationBridge::SelectedIdentificationBridge(QObject * parent)
	: Bridge(parent) {
}

SelectedIdentificationBridge::~SelectedIdentificationBridge() {}

bool SelectedIdentificationBridge::isActive() const {
	return d_identification.get() != NULL;
}

void SelectedIdentificationBridge::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	d_experiment = experiment;
}

void SelectedIdentificationBridge::setIdentification(const fmp::Identification::Ptr & identification) {
	setModified(false);
	d_identification = identification;
	if ( d_identification ) {
		qInfo() << "Selected Identification " << ToQString(identification);
	}

	emit useGlobalSizeChanged(useGlobalSize());
	emit tagSizeChanged(tagSize());
	emit startModified(start());
	emit endModified(end());
	emit activated(d_identification.get() != NULL);

}

fm::Time::ConstPtr SelectedIdentificationBridge::start() const {
	if ( !d_identification ) {
		return fm::Time::ConstPtr();
	}
	return d_identification->Start();
}

fm::Time::ConstPtr SelectedIdentificationBridge::end() const {
	if ( !d_identification ) {
		return fm::Time::ConstPtr();
	}
	return d_identification->End();
}


void SelectedIdentificationBridge::setStart(const fm::Time::ConstPtr & start) {
	if ( !d_identification
	     || TimePtrEquals(d_identification->Start(),start) == true ) {
		return;
	}

	try {
		qDebug() << "[SelectedIdentificationBridge]: Calling fort::myrmidon::priv::Identification::SetStart('"
		         << ToQString(start,"-") << "')";
		d_identification->SetStart(start);
	} catch ( const std::exception & e) {
		qCritical() << "Could not set identification start: " << e.what();
		emit startModified(d_identification->Start());
		return;
	}
	qInfo() << "Set identification start to " << ToQString(start,"-");
	setModified(true);
	emit startModified(start);
	emit identificationModified(d_identification);
}

void SelectedIdentificationBridge::setEnd(const fm::Time::ConstPtr & end ) {
	if ( !d_identification
	     || TimePtrEquals(d_identification->End(),end) == true ) {
		return;
	}

	try {
		qDebug() << "[SelectedIdentificationBridge]: Calling fort::myrmidon::priv::Identification::SetEnd('"
		         << ToQString(end,"+") << "')";
		d_identification->SetEnd(end);
	} catch ( const std::exception & e) {
		qCritical() << "Could not set identification end: " << e.what();
		emit endModified(d_identification->End());
		return;
	}

	qInfo() << "Set identification end to " << ToQString(end,"+");
	setModified(true);
	emit endModified(end);
	emit identificationModified(d_identification);
}


bool SelectedIdentificationBridge::useGlobalSize() const {
	if ( !d_identification ) {
		return true;
	}

	return d_identification->UseDefaultTagSize();
}

double SelectedIdentificationBridge::tagSize() const {
	if ( !d_identification ) {
		return 0.0;
	}
	if ( d_identification->UseDefaultTagSize() == true ) {
		return d_experiment->DefaultTagSize();
	}
	return d_identification->TagSize();
}

void SelectedIdentificationBridge::setTagSize(double tagSize) {
	if ( !d_identification
	     || d_identification->UseDefaultTagSize() == true
	     || tagSize == d_identification->TagSize() ) {
		return;
	}
	d_identification->SetTagSize(tagSize);
	setModified(true);
	emit tagSizeChanged(tagSize);
}

void SelectedIdentificationBridge::setUseGlobalSize(bool useGlobalSize) {
	if ( !d_identification ) {
		return;
	}
	if (useGlobalSize == d_identification->UseDefaultTagSize() ) {
		return;
	}
	if ( useGlobalSize == true) {
		d_identification->SetTagSize(fmp::Identification::DEFAULT_TAG_SIZE);
		emit tagSizeChanged(tagSize());
	} else {
		d_identification->SetTagSize(d_experiment->DefaultTagSize());
	}
	setModified(true);
	emit useGlobalSizeChanged(useGlobalSize);
}
