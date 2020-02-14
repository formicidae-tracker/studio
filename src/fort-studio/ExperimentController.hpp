#pragma once

#include "Bridge.hpp"
#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "ExperimentBridge.hpp"
#include "IdentifierBridge.hpp"
#include "SelectedAntBridge.hpp"
#include "SelectedIdentificationBridge.hpp"

namespace fmp = fort::myrmidon::priv;

class ExperimentController : public Bridge {
	Q_OBJECT
public:

	ExperimentController(QObject * parent = NULL);

	const fs::path & absoluteFilePath() const;

	bool isActive() const override;

	bool open(const QString & path);

	bool create(const QString & path);

	UniverseBridge * universe();

	MeasurementBridge * measurements();

	IdentifierBridge * identfier();

	ExperimentBridge * experiment();

	SelectedAntBridge * selectedAnt();

	SelectedIdentificationBridge * selectedIdentification();


public slots:

	bool save();
	bool saveAs(const QString & path);

private slots:
	void onChildModified(bool);
private:

	void setExperiment(const fmp::Experiment::Ptr & );
	void connectModifications();


	fmp::Experiment::Ptr           d_experiment;
	UniverseBridge               * d_universe;
	MeasurementBridge            * d_measurements;
	IdentifierBridge             * d_identifier;
	ExperimentBridge             * d_expBridge;
	SelectedAntBridge            * d_selectedAnt;
	SelectedIdentificationBridge * d_selectedIdentification;
};
