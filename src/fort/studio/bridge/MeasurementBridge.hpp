#pragma once

#include <QStandardItemModel>
#include <QFutureWatcher>

#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>

#include "Bridge.hpp"
#include <fort/studio/MyrmidonTypes/Types.hpp>
#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Measurement.hpp>
#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>

#include <tuple>



class MeasurementBridge : public GlobalBridge {
	Q_OBJECT
public:
	MeasurementBridge(QObject * parent);
	virtual ~MeasurementBridge();

	QAbstractItemModel * typeModel() const;

	fmp::MeasurementConstPtr measurementForCloseUp(const std::string & tcuURI,
	                                               fmp::MeasurementTypeID typeID);

	void initialize(ExperimentBridge * experiment) override;

signals:
	void measurementCreated(const fmp::Measurement::ConstPtr &m);
	void measurementModified(const fmp::Measurement::ConstPtr &m);
	void measurementDeleted(QString tcuURI, quint32 mtID);

	void measurementTypeModified(quint32,QString);
	void measurementTypeDeleted(quint32);


public slots:
	bool setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
	                    fmp::MeasurementType::ID mtID,
	                    QPointF start,
	                    QPointF end);

	void deleteMeasurement(const std::string & mURI);

	void setMeasurementType(quint32 mtID, const QString & name);

	void deleteMeasurementType(quint32 mtID);

	void deleteMeasurementType(const QModelIndex & index);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:
	void onTypeItemChanged(QStandardItem * item);

private:
	QList<QStandardItem*> buildType(const fmp::MeasurementType::Ptr & type) const;

	QStandardItemModel * d_typeModel;
};
