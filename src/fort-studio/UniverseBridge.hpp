#pragma once

#include <QObject>
#include <QStandardItemModel>


#include "Error.hpp"
#include <myrmidon/priv/Experiment.hpp>


namespace fmp = fort::myrmidon::priv;

class QAbstractItemModel;

class UniverseBridge : public QObject {
	Q_OBJECT

public:
	UniverseBridge(QObject * parent);

	QAbstractItemModel * model();
	void SetExperiment(const fmp::Experiment::Ptr & experiment);

	const std::vector<fmp::Space::Ptr> & Spaces() const;

	const fmp::Space::Universe::TrackingDataDirectoryByURI &
	TrackingDataDirectories() const;

public slots:
	Error addSpace(const QString & spaceName);
	Error addTrackingDataDirectoryToSpace(const QString & spaceURI,
	                                    const fmp::TrackingDataDirectoryConstPtr & tdd);
	Error deleteTrackingDataDirectory(const QString & URI);

signals:
	void spaceDeleted(const QString & spaceName);
	void spaceAdded(const fmp::Space::Ptr & space);
	void spaceChanged(const fmp::Space::Ptr & space);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void trackingDataDirectoryDeleted(const QString & URI);

protected slots:
	void on_model_itemChanged(QStandardItem * item);

private:
	const static std::vector<fmp::Space::Ptr> s_emptySpaces;
	const static fmp::Space::Universe::TrackingDataDirectoryByURI s_emptyTDDs;

	enum ObjectType {
	                 SPACE_TYPE = 0,
	                 TDD_TYPE  = 1,
	};

	QStandardItem * LocateSpace(const QString & URI);
	void RebuildSpaceChildren(QStandardItem * item, const fmp::Space::Ptr & z);

	QList<QStandardItem*> BuildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd);
	QList<QStandardItem*> BuildSpace(const fmp::Space::Ptr & z);

	void BuildAll(const std::vector<fmp::Space::Ptr> & spaces);


	QStandardItemModel   * d_model;
	fmp::Experiment::Ptr   d_experiment;
};
