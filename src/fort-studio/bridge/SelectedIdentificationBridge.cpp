#include "SelectedIdentificationBridge.hpp"

#include <QDebug>

#include <fort-studio/Format.hpp>

SelectedIdentificationBridge::SelectedIdentificationBridge(QObject * parent)
	: Bridge(parent) {
}

bool SelectedIdentificationBridge::isActive() const {
	return d_identification.get() != NULL;
}

void SelectedIdentificationBridge::setIdentification(const fmp::Identification::Ptr & identification) {
	setModified(false);
	d_identification = identification;
	if ( d_identification ) {
		qInfo() << "Selected Identification " << ToQString(identification);
	}

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

bool TimePtrEquals(const fm::Time::ConstPtr & a,
                   const fm::Time::ConstPtr & b) {
	if ( !a ) {
		return !b;
	}
	if (!b) {
		return false;
	}
	return a->Equals(*b);
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
		return;
	}

	qInfo() << "Set identification end to " << ToQString(end,"+");
	setModified(true);
	emit endModified(end);
	emit identificationModified(d_identification);
}
