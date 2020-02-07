#pragma once

#include <QObject>
#include <QStandardItemModel>


#include "Error.hpp"
#include <myrmidon/priv/Experiment.hpp>


namespace fmp = fort::myrmidon::priv;

class QAbstractItemModel;

class ZoneAndTDDBridge : public QObject {
	Q_OBJECT

public:
	ZoneAndTDDBridge(QObject * parent);

	QAbstractItemModel * model();
	void SetExperiment(fmp::Experiment * experiment);

	const std::vector<fmp::Zone::Ptr> Zones() const;

	const fmp::Zone::Group::TrackingDataDirectoryByURI &
	TrackingDataDirectories() const;

public slots:
	Error addZone(const QString & zoneName);
	Error addTrackingDataDirectoryToZone(const QString & zoneURI,
	                                    const fmp::TrackingDataDirectoryConstPtr & tdd);
	Error deleteTrackingDataDirectory(const QString & URI);

signals:
	void zoneDeleted(const QString & zoneName);
	void zoneAdded(const fmp::Zone::Ptr & zone);
	void zoneChanged(const fmp::Zone::Ptr & zone);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void trackingDataDirectoryDeleted(const QString & URI);

protected slots:
	void on_model_itemChanged(QStandardItem * item);

private:
	enum ObjectType {
	                 ZONE_TYPE = 0,
	                 TDD_TYPE  = 1,
	};

	QStandardItem * LocateZone(const QString & URI);
	void RebuildZoneChildren(QStandardItem * item, const fmp::Zone::Ptr & z);

	QList<QStandardItem*> BuildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd);
	QList<QStandardItem*> BuildZone(const fmp::Zone::Ptr & z);

	void BuildAll(const std::vector<fmp::Zone::Ptr> & zones);


	QStandardItemModel   * d_model;
	fmp::Experiment      * d_experiment;
};
