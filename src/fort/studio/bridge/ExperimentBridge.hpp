#pragma once

#include "Bridge.hpp"
#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "GlobalPropertyBridge.hpp"
#include "IdentifierBridge.hpp"
#include "SelectedAntBridge.hpp"
#include "SelectedIdentificationBridge.hpp"
#include "IdentifiedFrameConcurrentLoader.hpp"
#include "AntShapeTypeBridge.hpp"
#include "AntMetadataBridge.hpp"
#include "MovieBridge.hpp"
#include "IdentifiedFrameConcurrentLoader.hpp"
#include "ZoneBridge.hpp"
#include "StatisticsBridge.hpp"

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

	AntShapeTypeBridge * antShapeTypes() const;

	AntMetadataBridge * antMetadata() const;

	MovieBridge * movies() const;

	ZoneBridge * zones() const;

	StatisticsBridge * statistics() const;
public slots:

	bool save();
	bool saveAs(const QString & path);

private slots:
	void onChildModified(bool);
private:

	void setExperiment(const fmp::Experiment::Ptr & );
	void connectModifications();
	void resetChildModified();

	fmp::Experiment::Ptr              d_experiment;
	UniverseBridge                  * d_universe;
	MeasurementBridge               * d_measurements;
	IdentifierBridge                * d_identifier;
	GlobalPropertyBridge            * d_globalProperties;
	IdentifiedFrameConcurrentLoader * d_identifiedFrameLoader;
	AntShapeTypeBridge              * d_antShapeTypes;
	AntMetadataBridge               * d_antMetadata;
	MovieBridge                     * d_movies;
	ZoneBridge                      * d_zones;
	StatisticsBridge                * d_statistics;
};
