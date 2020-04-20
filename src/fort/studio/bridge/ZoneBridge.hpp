#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

#include <QList>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;

// class ZoneDefinitionBridge : public Bridge {
// 	Bridge(QObject * parent);

// 	void setZone(const fmp::Zone::Ptr & zone);


// };

class SpaceBridge : public Bridge {
	Q_OBJECT
public :
	SpaceBridge(QObject * parent);

	void setSpace(const fmp::Space::Ptr & space);

	QAbstractItemModel * snapshotModel() const;
	QAbstractItemModel * zoneModel() const;

private:
	QStandardItemModel  * d_snapshotModel;
	fmp::Experiment::Ptr  d_experiment;
};

class ZoneBridge : public Bridge {
	Q_OBJECT
public :
	ZoneBridge(QObject * parent);

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	SpaceBridge * selectedSpace() const;

	QAbstractItemModel * spaceModel() const;


	bool canAddItemAt(const QModelIndex & index);
	bool canRemoveItemAt(const QModelIndex & index);



public slots:
	void addItemAtIndex(const QModelIndex & index);
	void removeItemAtIndex(const QModelIndex & index);


	void rebuildSpaces();
	void onTrackingDataDirectoryChange(const QString & uri);

private:
	const static int TypeRole;
	const static int DataRole;

	const static int SpaceType       = 1;
	const static int ZoneType        = 2;
	const static int DefinitionType  = 3;

	void addZone(QStandardItem * spaceRootItem);
	void addDefinition(QStandardItem * zoneRootItem);

	void removeZone(QStandardItem * zoneItem);
	void removeDefinition(QStandardItem * zoneItem);

	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & space) const;
	QList<QStandardItem*> buildZone(const fmp::Zone::Ptr & zone) const;
	QList<QStandardItem*> buildDefinition(const fmp::Zone::Definition::Ptr & pdefinition) const;

	QStandardItemModel  * d_spaceModel;
	fmp::Experiment::Ptr  d_experiment;
};
