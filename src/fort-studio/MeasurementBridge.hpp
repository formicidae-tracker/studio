#pragma once

#include <QStandardItemModel>
#include <QFutureWatcher>

#include <myrmidon/priv/TagCloseUp.hpp>
#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Measurement.hpp>

namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::TagCloseUp::ConstPtr)
Q_DECLARE_METATYPE(fmp::MeasurementType::ID)


class TagCloseUpLoader : public QObject {
	Q_OBJECT;
public:
	TagCloseUpLoader(const fmp::TrackingDataDirectoryConstPtr & tdd,
	                 fort::tags::Family f,
	                 uint8_t threshold,
	                 QObject * parent);

	void waitForFinished();

	static fmp::TagCloseUp::List Load(const fmp::TagCloseUp::Lister::Loader & l);

signals:
	void newTagCloseUp(fs::path tddURI,
	                   fort::tags::Family family,
	                   uint8_t threshold,
	                   fmp::TagCloseUp::ConstPtr tcu);

public slots:
	void cancel();
	void start();

private slots:
	void onResultReady(int index);


private :
	fs::path                                d_tddURI;
	QFutureWatcher<fmp::TagCloseUp::List> * d_futureWatcher;
	fmp::TagCloseUp::Lister::Ptr            d_lister;
};


class MeasurementBridge : public QObject {
	Q_OBJECT
public:
	MeasurementBridge(QObject * parent);

	QAbstractItemModel * model() const;

	void SetExperiment(fmp::Experiment * experiment);


signals:
	void progressChanged(size_t done, size_t toDo);

	void measurementModified(const fmp::MeasurementConstPtr);
	void measurementDeleted(fs::path);

public slots:
	void onTDDAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void onTDDDeleted(const QString &);

	void onFamilyChanged(fort::tags::Family f);
	void onThresholdChanged(uint8_t threshold);


	void setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
	                    fmp::MeasurementType::ID MTID,
	                    QPointF start,
	                    QPointF end);

	void deleteMeasurement(const fs::path & mURI);


private slots:

	void onNewTagCloseUp(fs::path tddURI,
	                     fort::tags::Family f,
	                     uint8_t Threshold,
	                     fmp::TagCloseUp::ConstPtr tcu);


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


	QList<QStandardItem*> BuildTag(fmp::TagID TID);
	QList<QStandardItem*> BuildTCU(const fmp::TagCloseUp::ConstPtr & tcu);

	size_t countMeasurementsForTCU(const fs::path & tcuPath);

	QStandardItemModel * d_model;
	fmp::Experiment    * d_experiment;
	CountByTcuURI        d_counts;
	CloseUpByTddURI      d_closeups;
	LoaderByTddURI       d_loaders;

};
