#pragma once

#include <QStandardItemModel>
#include <QFutureWatcher>

#include <myrmidon/priv/TagCloseUp.hpp>
#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Measurement.hpp>

#include "Bridge.hpp"
#include "MyrmidonTypes.hpp"

class TagCloseUpLoader : public QObject {
	Q_OBJECT;
public:
	TagCloseUpLoader(const fmp::TrackingDataDirectoryConstPtr & tdd,
	                 fort::tags::Family f,
	                 uint8_t threshold,
	                 QObject * parent);

	void waitForFinished();

	static fmp::TagCloseUp::List load(const fmp::TagCloseUp::Lister::Loader & l);

	QAbstractItemModel * tagCloseUpModel() const;
	QAbstractItemModel * measurementTypeModel() const;

	size_t toDo() const;
	size_t done() const;
signals:
	void newTagCloseUp(fs::path tddURI,
	                   fort::tags::Family family,
	                   uint8_t threshold,
	                   fmp::TagCloseUp::ConstPtr tcu);

	void progressChanged(size_t done, size_t oldDone);
public slots:
	void cancel();
	void start();

private slots:
	void onResultReady(int index);


private :
	fs::path                                d_tddURI;
	QFutureWatcher<fmp::TagCloseUp::List> * d_futureWatcher;
	fmp::TagCloseUp::Lister::Ptr            d_lister;
	size_t                                  d_done,d_toDo;
};


class MeasurementBridge : public Bridge {
	Q_OBJECT
public:
	MeasurementBridge(QObject * parent);

	QAbstractItemModel * model() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

signals:
	void progressChanged(size_t done, size_t toDo);

	void measurementModified(const fmp::MeasurementConstPtr);
	void measurementDeleted(fs::path);

	void measurementTypeModified(int,QString);
	void measurementTypeDeleted(int);

public slots:
	void onTDDAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void onTDDDeleted(const QString &);

	void onDetectionSettingChanged(fort::tags::Family f, uint8_t threshold);

	void setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
	                    fmp::MeasurementType::ID MTID,
	                    QPointF start,
	                    QPointF end);

	void deleteMeasurement(const fs::path & mURI);


	void setMeasurementType(int MTID, const QString & name);

	void deleteMeasurementType(int MTID);


private slots:

	void onNewTagCloseUp(fs::path tddURI,
	                     fort::tags::Family f,
	                     uint8_t Threshold,
	                     fmp::TagCloseUp::ConstPtr tcu);

	void onLoaderProgressChanged(size_t done, size_t oldDone);

	void onTypeItemChanged(QStandardItem * item);


private:
	typedef std::map<fs::path,fmp::TagCloseUp::ConstPtr> CloseUpByPath;
	typedef std::map<fs::path,CloseUpByPath>             CloseUpByTddURI;
	typedef std::map<fs::path,TagCloseUpLoader*>         LoaderByTddURI;
	typedef std::map<fs::path,QStandardItem*>            CountByTcuURI;

	void startAll();
	void startOne(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void cancelAll();
	void cancelOne(const fs::path & tddURI);

	void addOneTCU(const fs::path & tddURI, const fmp::TagCloseUp::ConstPtr & tcu);
	void clearTddTCUs(const fs::path & tddURI);
	void clearAllTCUs();


	QList<QStandardItem*> buildTag(fmp::TagID TID) const;
	QList<QStandardItem*> buildTCU(const fmp::TagCloseUp::ConstPtr & tcu);
	QList<QStandardItem*> buildType(const fmp::MeasurementType::Ptr & type) const;

	size_t countMeasurementsForTCU(const fs::path & tcuPath) const;

	QStandardItemModel * d_tcuModel;
	QStandardItemModel * d_typeModel;
	fmp::Experiment::Ptr d_experiment;
	CountByTcuURI        d_counts;
	CloseUpByTddURI      d_closeups;
	LoaderByTddURI       d_loaders;
	size_t               d_toDo,d_done;
};
