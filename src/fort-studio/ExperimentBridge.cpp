#include "ExperimentBridge.hpp"


ExperimentBridge::ExperimentBridge(QObject * parent)
	: QObject(parent) {
}


void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
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


QString ExperimentBridge::name() const {
	if ( !d_experiment ) {
		return "";
	}
	return d_experiment->Name().c_str();
}

QString ExperimentBridge::author() const {
	if ( !d_experiment ) {
		return "";
	}
	return d_experiment->Author().c_str();
}

QString ExperimentBridge::comment() const {
	if ( !d_experiment ) {
		return "";
	}

	return d_experiment->Comment().c_str();
}

fort::tags::Family ExperimentBridge::tagFamily() const {
	if ( !d_experiment ) {
		return fort::tags::Family::Undefined;
	}

	return d_experiment->Family();
}

uint8_t ExperimentBridge::threshold() const {
	if ( !d_experiment ) {
		return 255;
	}

	return d_experiment->Threshold();
}

double ExperimentBridge::tagSize() const {
	if ( !d_experiment ) {
		return 0.0;
	}

	return d_experiment->DefaultTagSize();
}

void ExperimentBridge::setName(const QString & name) {
	if ( !d_experiment || d_experiment->Name().c_str() == name ) {
		return;
	}
	d_experiment->SetName(name.toUtf8().data());
	emit nameChanged(name);
}

void ExperimentBridge::setAuthor(const QString & author) {
	if ( !d_experiment || d_experiment->Author().c_str() == author ) {
		return;
	}
	d_experiment->SetAuthor(author.toUtf8().data());
	emit authorChanged(author);
}

void ExperimentBridge::setComment(const QString & comment) {
	if ( !d_experiment || d_experiment->Comment().c_str() == comment ) {
		return;
	}

	d_experiment->SetComment(comment.toUtf8().data());

	emit commentChanged(comment);
}

void ExperimentBridge::setThreshold(uint8_t th) {
	if ( !d_experiment || d_experiment->Threshold() == th ) {
		return;
	}

	auto old = d_experiment->Threshold();
	d_experiment->SetThreshold(th);
	if ( old != d_experiment->Threshold() ) {
		emit thresholdChanged(d_experiment->Threshold());
		emit detectionSettingChanged(d_experiment->Family(),d_experiment->Threshold());
	}
}

void ExperimentBridge::setTagSize(double tagSize) {
	if ( !d_experiment || d_experiment->DefaultTagSize() == tagSize) {
		return;
	}
	d_experiment->SetDefaultTagSize(tagSize);
	emit tagSizeChanged(tagSize);
}

void ExperimentBridge::setTagFamily(fort::tags::Family tf) {
	if ( !d_experiment || d_experiment->Family() == tf) {
		return;
	}
	d_experiment->SetFamily(tf);
	emit tagFamilyChanged(tf);
	emit detectionSettingChanged(d_experiment->Family(),d_experiment->Threshold());
}
