#pragma once

#include <QStandardItemModel>
#include <QFutureWatcher>

#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>

#include "Bridge.hpp"
#include <fort/studio/MyrmidonTypes.hpp>

#include <tuple>

class TagCloseUpLoader {
public:
	typedef std::tuple<size_t,std::string,fmp::TagCloseUp::List> Result;
	TagCloseUpLoader(const fmp::TagCloseUp::Lister::Loader & loader,
	                 const std::string & tddURI,
	                 size_t seed);

	Result load() const;

private :
	fmp::TagCloseUp::Lister::Loader d_loader;
	std::string                     d_tddURI;
	size_t                          d_seed;
};


class MeasurementBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(bool isReady
	           READ isReady
	           NOTIFY ready)
	Q_PROPERTY(bool isOutdated
	           READ isOutdated
	           NOTIFY outdated)
public:
	MeasurementBridge(QObject * parent);
	virtual ~MeasurementBridge();
	void setExperiment(const fmp::Experiment::Ptr & experiment);

	QAbstractItemModel * tagCloseUpModel() const;
	QAbstractItemModel * measurementTypeModel() const;

	bool isActive() const override;

	fmp::TagCloseUp::ConstPtr fromTagCloseUpModelIndex(const QModelIndex & index);

	fmp::MeasurementConstPtr measurement(const std::string & tcuURI,
	                                     fmp::MeasurementTypeID typeID);

	void queryTagCloseUp(QVector<fmp::TagCloseUp::ConstPtr> & tcus,
	                     const fmp::IdentificationConstPtr & identification);

	bool isReady() const;

	bool isOutdated() const;

signals:
	void progressChanged(size_t done, size_t toDo);

	void measurementModified(const fmp::Measurement::ConstPtr &m);
	void measurementDeleted(QString tcuURI, quint32 mtID);

	void measurementTypeModified(quint32,QString);
	void measurementTypeDeleted(quint32);


	void ready(bool);
	void outdated(bool);

public slots:
	void onTDDAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void onTDDDeleted(const QString &);

	void onDetectionSettingChanged(fort::tags::Family f, uint8_t threshold);

	void loadTagCloseUp();

	void setMeasurement(const fmp::TagCloseUp::ConstPtr & tcu,
	                    fmp::MeasurementType::ID MTID,
	                    QPointF start,
	                    QPointF end);

	void deleteMeasurement(const std::string & mURI);

	void setMeasurementType(quint32 MTID, const QString & name);

	void deleteMeasurementType(quint32 MTID);

	void deleteMeasurementType(const QModelIndex & index);
private slots:

	void onTypeItemChanged(QStandardItem * item);


private:
	typedef std::map<std::string,fmp::TagCloseUp::ConstPtr> CloseUpByPath;
	typedef std::map<std::string,CloseUpByPath>             CloseUpByTddURI;
	typedef std::map<std::string,QStandardItem*>            CountByTcuURI;

	void cancelAll();

	void addOneTCU(const std::string & tddURI, const fmp::TagCloseUp::ConstPtr & tcu);
	void clearTddTCUs(const std::string & tddURI);
	void clearAllTCUs();
	void setOutdated(bool v);

	QList<QStandardItem*> buildTag(fmp::TagID TID) const;
	QList<QStandardItem*> buildTCU(const fmp::TagCloseUp::ConstPtr & tcu);
	QList<QStandardItem*> buildType(const fmp::MeasurementType::Ptr & type) const;

	size_t countMeasurementsForTCU(const std::string & tcuURI) const;

	QStandardItemModel * d_tcuModel;
	QStandardItemModel * d_typeModel;
	fmp::Experiment::Ptr d_experiment;
	CountByTcuURI        d_counts;
	CloseUpByTddURI      d_closeups;
	size_t               d_seed;
	bool                 d_outdated;

	QFutureWatcher<TagCloseUpLoader::Result> *  d_watcher;
	std::vector<TagCloseUpLoader>               d_loaders;
};
