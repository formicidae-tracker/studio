#pragma once

#include "Bridge.hpp"
#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Space.hpp>

class QAbstractItemModel;

class UniverseBridge : public Bridge {
	Q_OBJECT

public:

	UniverseBridge(QObject * parent);
	virtual ~UniverseBridge();

	QAbstractItemModel * model();
	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isDeletable(const QModelIndexList & ) const;

	bool isActive() const override;

	QString basepath() const;

	const fmp::Space::Universe::TrackingDataDirectoryByURI & trackingDataDirectories() const;

	std::map<quint32,QString> spaceNamesByID() const;

public slots:
	void addSpace(const QString & spaceName);
	void deleteSpace(const QString & spaceName);
	void addTrackingDataDirectoryToSpace(const QString & spaceName,
	                                     const fmp::TrackingDataDirectory::Ptr & tdd);
	void deleteTrackingDataDirectory(const QString & URI);


	void deleteSelection(const QModelIndexList & selected);

signals:
	void spaceDeleted(const QString & spaceName);
	void spaceAdded(const fmp::Space::Ptr & space);
	void spaceChanged(const fmp::Space::Ptr & space);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd);
	void trackingDataDirectoryDeleted(const QString & URI);

private slots:
	void onItemChanged(QStandardItem * item);

private:

	const static std::vector<fmp::Space::Ptr> s_emptySpaces;
	const static fmp::Space::Universe::TrackingDataDirectoryByURI s_emptyTDDs;

	enum ObjectType {
	                 SPACE_TYPE = 1,
	                 TDD_TYPE  = 2,
	};

	QStandardItem * locateSpace(const QString & URI);
	void rebuildSpaceChildren(QStandardItem * item, const fmp::Space::Ptr & z);

	QList<QStandardItem*> buildTDD(const fmp::TrackingDataDirectory::Ptr & tdd);
	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & z);

	void rebuildAll(const fmp::SpaceByID & spaces);

	QStandardItemModel   * d_model;
	fmp::Experiment::Ptr   d_experiment;
};
