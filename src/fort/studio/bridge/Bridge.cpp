#include "Bridge.hpp"


Bridge::~Bridge() {}

Bridge::Bridge(QObject * parent)
	: d_modified(false) {
}

bool Bridge::isModified() const {
	return d_modified;
}

void Bridge::setModified(bool modifiedValue) {
	if ( d_modified == modifiedValue ){
		return;
	}
	d_modified = modifiedValue;
	emit modified(d_modified);
}


GlobalBridge::GlobalBridge(QObject * parent)
	: Bridge(parent) {
}

GlobalBridge::~GlobalBridge() {
}

bool GlobalBridge::isActive() const {
	return d_experiment.get() != nullptr;
}

void GlobalBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	tearDownExperiment();
	d_experiment = experiment;
	setUpExperiment();
	setModified(false);
	emit activated(isActive());
}
