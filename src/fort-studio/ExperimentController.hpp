#pragma once

#include <QObject>

#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "ExperimentBridge.hpp"
#include "IdentifierBridge.hpp"
#include "SelectedAntBridge.hpp"
#include "SelectedIdentificationBridge.hpp"

namespace fmp = fort::myrmidon::priv;

class ExperimentController : public QObject {
	Q_OBJECT
public:

	ExperimentController(QObject * parent = NULL);

	bool isModified() const;

	void open(const QString & path);

	void create(const QString & path);

	UniverseBridge * universe();

	MeasurementBridge * measurements();

	IdentifierBridge * identfier();

	ExperimentBridge * experiment();

	SelectedAntBridge * selectedAnt();

	SelectedIdentificationBridge * selectedIdentification();

signals:
	void modified(bool);

public slots:

	void save(const QString & path);
	void setModified(bool);

private slots:
	void setModifiedTrue();
private:

	void setExperiment(const fmp::Experiment::Ptr & );
	void connectModifications();



	fmp::Experiment::Ptr           d_experiment;
	bool                           d_modified;
	UniverseBridge               * d_universe;
	MeasurementBridge            * d_measurements;
	IdentifierBridge             * d_identifier;
	ExperimentBridge             * d_expBridge;
	SelectedAntBridge            * d_selectedAnt;
	SelectedIdentificationBridge * d_selectedIdentification;
};
