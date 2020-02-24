#pragma once

#include "Bridge.hpp"
#include <QStandardItemModel>

#include <myrmidon/priv/Experiment.hpp>

#include <fort-studio/MyrmidonTypes.hpp>

class QAbstractItemModel;

class UniverseBridge : public Bridge {
	Q_OBJECT

public:

	UniverseBridge(QObject * parent);

	QAbstractItemModel * model();
	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isDeletable(const QModelIndexList & ) const;

	bool isActive() const override;

	QString basepath() const;

public slots:
	void addSpace(const QString & spaceName);
	void deleteSpace(const QString & spaceName);
	void addTrackingDataDirectoryToSpace(const QString & spaceName,
	                                     const fmp::TrackingDataDirectory::ConstPtr & tdd);
	void deleteTrackingDataDirectory(const QString & URI);


	void deleteSelection(const QModelIndexList & selected);

signals:
	void spaceDeleted(const QString & spaceName);
	void spaceAdded(const fmp::Space::Ptr & space);
	void spaceChanged(const fmp::Space::Ptr & space);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectory::ConstPtr & tdd);
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

	QList<QStandardItem*> buildTDD(const fmp::TrackingDataDirectory::ConstPtr & tdd);
	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & z);

	void buildAll(const std::vector<fmp::Space::Ptr> & spaces);

	QStandardItemModel   * d_model;
	fmp::Experiment::Ptr   d_experiment;
};
