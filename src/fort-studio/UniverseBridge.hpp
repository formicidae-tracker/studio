#pragma once

#include <QObject>
#include <QStandardItemModel>


#include "Error.hpp"
#include <myrmidon/priv/Experiment.hpp>


namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::TrackingDataDirectoryConstPtr);
Q_DECLARE_METATYPE(fmp::Space::Ptr);


class QAbstractItemModel;

class UniverseBridge : public QObject {
	Q_OBJECT

public:
	UniverseBridge(QObject * parent);

	QAbstractItemModel * model();
	void setExperiment(const fmp::Experiment::Ptr & experiment);

	const std::vector<fmp::Space::Ptr> & spaces() const;

	const fmp::Space::Universe::TrackingDataDirectoryByURI &
	trackingDataDirectories() const;

public slots:
	Error addSpace(const QString & spaceName);
	Error addTrackingDataDirectoryToSpace(const QString & spaceURI,
	                                    const fmp::TrackingDataDirectoryConstPtr & tdd);
	Error deleteTrackingDataDirectory(const QString & URI);

signals:
	void activeStateChanged(bool);

	void spaceDeleted(const QString & spaceName);
	void spaceAdded(const fmp::Space::Ptr & space);
	void spaceChanged(const fmp::Space::Ptr & space);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectoryConstPtr & tdd);
	void trackingDataDirectoryDeleted(const QString & URI);

private slots:
	void onItemChanged(QStandardItem * item);

private:
	const static std::vector<fmp::Space::Ptr> s_emptySpaces;
	const static fmp::Space::Universe::TrackingDataDirectoryByURI s_emptyTDDs;

	enum ObjectType {
	                 SPACE_TYPE = 0,
	                 TDD_TYPE  = 1,
	};

	QStandardItem * locateSpace(const QString & URI);
	void rebuildSpaceChildren(QStandardItem * item, const fmp::Space::Ptr & z);

	QList<QStandardItem*> buildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd);
	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & z);

	void buildAll(const std::vector<fmp::Space::Ptr> & spaces);

	QStandardItemModel   * d_model;
	fmp::Experiment::Ptr   d_experiment;
};
