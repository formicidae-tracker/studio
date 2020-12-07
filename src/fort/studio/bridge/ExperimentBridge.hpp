#pragma once

#include "Bridge.hpp"

class UniverseBridge;
class MeasurementBridge;
class IdentifierBridge;
class AntDisplayBridge;
class GlobalPropertyBridge;
class SelectedAntBridge;
class IdentifiedFrameConcurrentLoader;
class AntShapeTypeBridge;
class AntMetadataBridge;
class MovieBridge;
class ZoneBridge;
class StatisticsBridge;
class TagCloseUpBridge;
class AntMeasurementBridge;
class AntShapeBridge;

class QWidget;

namespace fmp = fort::myrmidon::priv;


class ExperimentBridge : public Bridge {
	Q_OBJECT
public:

	ExperimentBridge(QObject * parent = NULL);
	virtual ~ExperimentBridge();
	const fs::path & absoluteFilePath() const;

	bool isActive() const override;

	bool open(const QString & path,
	          QWidget * parent = nullptr);

	bool create(const QString & path);

	UniverseBridge * universe() const;

	MeasurementBridge * measurements() const;

	IdentifierBridge * identifier() const;

	AntDisplayBridge * antDisplay() const;

	GlobalPropertyBridge * globalProperties() const;

	SelectedAntBridge * selectedAnt() const;

	IdentifiedFrameConcurrentLoader * identifiedFrameLoader() const;

	AntShapeTypeBridge * antShapeTypes() const;

	AntMetadataBridge * antMetadata() const;

	MovieBridge * movies() const;

	ZoneBridge * zones() const;

	StatisticsBridge * statistics() const;

	TagCloseUpBridge * tagCloseUps() const;

	AntMeasurementBridge * antMeasurements() const;

	AntShapeBridge * antShapes() const;

	fmp::Ant::ConstPtr ant(fmp::AntID antID) const;

	void setExperiment(const fmp::Experiment::Ptr & );

public slots:
	bool save();
	bool saveAs(const QString & path);

	fmp::Ant::Ptr createAnt();

	void deleteAnt(quint32 antID);
	void selectAnt(quint32 antID);

signals:
	void antCreated(quint32);
	void antDeleted(quint32);



private slots:
	void onChildModified(bool);
private:
	friend class ExperimentBridgeUTest_ActiveModifiedState_Test;

	void resetChildModified();


	fmp::Experiment::Ptr              d_experiment;
	UniverseBridge                  * d_universe;
	MeasurementBridge               * d_measurements;
	IdentifierBridge                * d_identifier;
	AntDisplayBridge                * d_antDisplay;
	SelectedAntBridge               * d_selectedAnt;
	GlobalPropertyBridge            * d_globalProperties;
	IdentifiedFrameConcurrentLoader * d_identifiedFrameLoader;
	AntShapeTypeBridge              * d_antShapeTypes;
	AntMetadataBridge               * d_antMetadata;
	MovieBridge                     * d_movies;
	ZoneBridge                      * d_zones;
	StatisticsBridge                * d_statistics;
	TagCloseUpBridge                * d_tagCloseUps;
	AntMeasurementBridge            * d_antMeasurements;
	AntShapeBridge                  * d_antShapes;
	const std::vector<GlobalBridge*>  d_children;

};
