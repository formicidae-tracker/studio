#pragma once

#include "Bridge.hpp"
#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "GlobalPropertyBridge.hpp"
#include "IdentifierBridge.hpp"
#include "SelectedAntBridge.hpp"
#include "SelectedIdentificationBridge.hpp"
#include "IdentifiedFrameConcurrentLoader.hpp"

namespace fmp = fort::myrmidon::priv;

class ExperimentBridge : public Bridge {
	Q_OBJECT
public:

	ExperimentBridge(QObject * parent = NULL);

	const fs::path & absoluteFilePath() const;

	bool isActive() const override;

	bool open(const QString & path);

	bool create(const QString & path);

	UniverseBridge * universe() const;

	MeasurementBridge * measurements() const;

	IdentifierBridge * identifier() const;

	GlobalPropertyBridge * globalProperties() const;

	SelectedAntBridge * selectedAnt() const;

	SelectedIdentificationBridge * selectedIdentification() const;

	IdentifiedFrameConcurrentLoader * identifiedFrameLoader() const;

public slots:

	bool save();
	bool saveAs(const QString & path);

private slots:
	void onChildModified(bool);
private:

	void setExperiment(const fmp::Experiment::Ptr & );
	void connectModifications();


	fmp::Experiment::Ptr              d_experiment;
	UniverseBridge                  * d_universe;
	MeasurementBridge               * d_measurements;
	SelectedAntBridge               * d_selectedAnt;
	IdentifierBridge                * d_identifier;
	GlobalPropertyBridge            * d_globalProperties;
	SelectedIdentificationBridge    * d_selectedIdentification;
	IdentifiedFrameConcurrentLoader * d_identifiedFrameLoader;
};
