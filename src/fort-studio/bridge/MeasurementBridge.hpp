#pragma once

#include <QStandardItemModel>
#include <QFutureWatcher>

#include <myrmidon/priv/TagCloseUp.hpp>
#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Measurement.hpp>

#include "Bridge.hpp"
#include <fort-studio/MyrmidonTypes.hpp>

class TagCloseUpLoader : public QObject {
	Q_OBJECT;
public:
	TagCloseUpLoader(const fmp::TrackingDataDirectoryConstPtr & tdd,
	                 fort::tags::Family f,
	                 uint8_t threshold,
	                 QObject * parent);

	void waitForFinished();

	static fmp::TagCloseUp::List load(fmp::TagCloseUp::Lister::Loader l);

	size_t toDo() const;
	size_t done() const;
signals:
	void newTagCloseUp(std::string tddURI,
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
	std::string                                  d_tddURI;
	QFutureWatcher<fmp::TagCloseUp::List>      * d_futureWatcher;
	fmp::TagCloseUp::Lister::Ptr                 d_lister;
	std::vector<fmp::TagCloseUp::Lister::Loader> d_loaders;
	size_t                                       d_done,d_toDo;
};


class MeasurementBridge : public Bridge {
	Q_OBJECT
public:
	MeasurementBridge(QObject * parent);

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	QAbstractItemModel * tagCloseUpModel() const;
	QAbstractItemModel * measurementTypeModel() const;

	bool isActive() const override;

	fmp::TagCloseUp::ConstPtr fromTagCloseUpModelIndex(const QModelIndex & index);

signals:
	void progressChanged(size_t done, size_t toDo);

	void measurementModified(const fmp::Measurement::ConstPtr &);
	void measurementDeleted(std::string);

	void measurementTypeModified(quint32,QString);
	void measurementTypeDeleted(quint32);

public slots:
	void onTDDAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void onTDDDeleted(const QString &);

	void onDetectionSettingChanged(fort::tags::Family f, uint8_t threshold);

	void setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
	                    fmp::MeasurementType::ID MTID,
	                    QPointF start,
	                    QPointF end);

	void deleteMeasurement(const std::string & mURI);

	void setMeasurementType(quint32 MTID, const QString & name);

	void deleteMeasurementType(quint32 MTID);

	void deleteMeasurementType(const QModelIndex & index);
private slots:

	void onNewTagCloseUp(std::string tddURI,
	                     fort::tags::Family f,
	                     uint8_t Threshold,
	                     fmp::TagCloseUp::ConstPtr tcu);

	void onLoaderProgressChanged(size_t done, size_t oldDone);

	void onTypeItemChanged(QStandardItem * item);


private:
	typedef std::map<std::string,fmp::TagCloseUp::ConstPtr> CloseUpByPath;
	typedef std::map<std::string,CloseUpByPath>             CloseUpByTddURI;
	typedef std::map<std::string,TagCloseUpLoader*>         LoaderByTddURI;
	typedef std::map<std::string,QStandardItem*>            CountByTcuURI;

	void startAll();
	void startOne(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void cancelAll();
	void cancelOne(const std::string & tddURI);

	void addOneTCU(const std::string & tddURI, const fmp::TagCloseUp::ConstPtr & tcu);
	void clearTddTCUs(const std::string & tddURI);
	void clearAllTCUs();


	QList<QStandardItem*> buildTag(fmp::TagID TID) const;
	QList<QStandardItem*> buildTCU(const fmp::TagCloseUp::ConstPtr & tcu);
	QList<QStandardItem*> buildType(const fmp::MeasurementType::Ptr & type) const;

	size_t countMeasurementsForTCU(const std::string & tcuURI) const;

	QStandardItemModel * d_tcuModel;
	QStandardItemModel * d_typeModel;
	fmp::Experiment::Ptr d_experiment;
	CountByTcuURI        d_counts;
	CloseUpByTddURI      d_closeups;
	LoaderByTddURI       d_loaders;
	size_t               d_toDo,d_done;
};
