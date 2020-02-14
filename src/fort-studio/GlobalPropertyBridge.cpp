#include "GlobalPropertyBridge.hpp"


GlobalPropertyBridge::GlobalPropertyBridge(QObject * parent)
	: Bridge(parent) {
}


void GlobalPropertyBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	setModified(false);
	if ( experiment == d_experiment ) {
		return;
	}
	d_experiment = experiment;
	emit nameChanged(name());
	emit authorChanged(author());
	emit commentChanged(comment());
	emit tagFamilyChanged(tagFamily());
	emit thresholdChanged(threshold());
	emit tagSizeChanged(tagSize());
	emit activated(d_experiment.get() != NULL);
}

bool GlobalPropertyBridge::isActive() const {
	return d_experiment.get() != NULL;
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

uint8_t GlobalPropertyBridge::threshold() const {
	if ( !d_experiment ) {
		return 255;
	}

	return d_experiment->Threshold();
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

void GlobalPropertyBridge::setComment(const QString & comment) {
	if ( !d_experiment || d_experiment->Comment().c_str() == comment ) {
		return;
	}

	d_experiment->SetComment(comment.toUtf8().data());
	setModified(true);
	emit commentChanged(comment);
}

void GlobalPropertyBridge::setThreshold(uint8_t th) {
	if ( !d_experiment || d_experiment->Threshold() == th ) {
		return;
	}

	auto old = d_experiment->Threshold();
	d_experiment->SetThreshold(th);
	if ( old != d_experiment->Threshold() ) {
		setModified(true);
		emit thresholdChanged(d_experiment->Threshold());
		emit detectionSettingChanged(d_experiment->Family(),d_experiment->Threshold());
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

void GlobalPropertyBridge::setTagFamily(fort::tags::Family tf) {
	if ( !d_experiment || d_experiment->Family() == tf) {
		return;
	}
	d_experiment->SetFamily(tf);
	setModified(true);
	emit tagFamilyChanged(tf);
	emit detectionSettingChanged(d_experiment->Family(),d_experiment->Threshold());
}
