#pragma once


#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/Measurement.hpp>

class QAbstractItemModel;
class AntGlobalModel;
class QStandardItem;

class AntMeasurementBridge : public GlobalBridge {
	Q_OBJECT
public:
	explicit AntMeasurementBridge(QObject * parent = nullptr);
	virtual ~AntMeasurementBridge();

	void initialize(ExperimentBridge * experiment) override;

	QAbstractItemModel * model() const;

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:
	void onMeasurementTypeModified(quint32 mtID, QString name);
	void onMeasurementTypeDeleted(quint32);

	void onAntCreated(quint32 antID);
	void onAntDeleted(quint32 antID);

	void onMeasurementCreated(const fmp::Measurement::ConstPtr & m);
	void onMeasurementDeleted(const fmp::Measurement::ConstPtr & m);

	void onIdentificationModified(fmp::Identification::ConstPtr identification);

private:
	std::pair<QStandardItem*,int> headerForType(quint32 mtID) const;


	void updateMeasurementCount(const fmp::Measurement::ConstPtr & m, int incrementValue);

	void buildCountForAnt(const fmp::Ant::ConstPtr & ant);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	AntGlobalModel * d_model;

};
