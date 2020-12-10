#include "GlobalPropertyBridge.hpp"

#include <QDebug>

#include "ExperimentBridge.hpp"
#include "UniverseBridge.hpp"

GlobalPropertyBridge::~GlobalPropertyBridge() {}

GlobalPropertyBridge::GlobalPropertyBridge(QObject * parent)
	: GlobalBridge(parent) {
	d_cached = fort::tags::Family::Undefined;
}

void GlobalPropertyBridge::initialize(ExperimentBridge * experiment) {
	connect(experiment->universe(),
	        &UniverseBridge::trackingDataDirectoryAdded,
	        this,
	        &GlobalPropertyBridge::onTDDModified);

	connect(experiment->universe(),
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        this,
	        &GlobalPropertyBridge::onTDDModified);

}

void GlobalPropertyBridge::tearDownExperiment() {
	d_cached = tagFamily();
}

void GlobalPropertyBridge::setUpExperiment() {
	d_cached = tagFamily();
	emit nameChanged(name());
	emit authorChanged(author());
	emit commentChanged(comment());
	emit tagFamilyChanged(tagFamily());
	emit tagSizeChanged(tagSize());
}

QString GlobalPropertyBridge::name() const {
	if ( !d_experiment ) {
		return "";
	}
	return d_experiment->Name().c_str();
}

QString GlobalPropertyBridge::author() const {
	if ( !d_experiment ) {
		return "";
	}
	return d_experiment->Author().c_str();
}

QString GlobalPropertyBridge::comment() const {
	if ( !d_experiment ) {
		return "";
	}

	return d_experiment->Comment().c_str();
}

fort::tags::Family GlobalPropertyBridge::tagFamily() const {
	if ( !d_experiment ) {
		return fort::tags::Family::Undefined;
	}
	return d_experiment->Family();
}

double GlobalPropertyBridge::tagSize() const {
	if ( !d_experiment ) {
		return 0.0;
	}

	return d_experiment->DefaultTagSize();
}

void GlobalPropertyBridge::setName(const QString & name) {
	if ( !d_experiment || d_experiment->Name().c_str() == name ) {
		return;
	}
	d_experiment->SetName(name.toUtf8().data());
	setModified(true);
	emit nameChanged(name);
}

void GlobalPropertyBridge::setAuthor(const QString & author) {
	if ( !d_experiment || d_experiment->Author().c_str() == author ) {
		return;
	}
	d_experiment->SetAuthor(author.toUtf8().data());
	setModified(true);
	emit authorChanged(author);
}

void GlobalPropertyBridge::setComment(const QString & comment, bool noSignal) {
	if ( !d_experiment || d_experiment->Comment().c_str() == comment ) {
		return;
	}

	d_experiment->SetComment(comment.toUtf8().data());
	setModified(true);
	if ( noSignal == false ) {
		emit commentChanged(comment);
	}
}

void GlobalPropertyBridge::setTagSize(double tagSize) {
	if ( !d_experiment || d_experiment->DefaultTagSize() == tagSize) {
		return;
	}
	d_experiment->SetDefaultTagSize(tagSize);
	setModified(true);
	emit tagSizeChanged(tagSize);
}


void GlobalPropertyBridge::onTDDModified() {
	if ( tagFamily() == d_cached ) {
		return;
	}
	d_cached = tagFamily();
	emit tagFamilyChanged(tagFamily());
}
