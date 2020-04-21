#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

#include <QList>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;


class ZoneBridge : public Bridge {
	Q_OBJECT
public :
	ZoneBridge(QObject * parent);

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

	QAbstractItemModel * spaceModel() const;
	QAbstractItemModel * zonesModel() const;
	QAbstractItemModel * fullFrameModel() const;


	bool canAddItemAt(const QModelIndex & index);
	bool canRemoveItemAt(const QModelIndex & index);

	struct FullFrame {
		fmp::FrameReference Reference;
		QString             AbsoluteFilePath;
	};

	std::pair<bool,FullFrame> fullFrameAtIndex(const QModelIndex & index) const;

public slots:
	void addItemAtIndex(const QModelIndex & index);
	void removeItemAtIndex(const QModelIndex & index);


	void rebuildSpaces();
	void onTrackingDataDirectoryChange(const QString & uri);

	void activateItem(QModelIndex index);

private slots:
	void onItemChanged(QStandardItem * item);

private:
	const static int TypeRole;
	const static int DataRole;

	const static int SpaceType       = 1;
	const static int ZoneType        = 2;
	const static int DefinitionType  = 3;

	void rebuildFullFrameModel();
	void rebuildZoneModel();

	QStandardItem * getSibling(QStandardItem * item,int column);

	void addZone(QStandardItem * spaceRootItem);
	void addDefinition(QStandardItem * zoneRootItem);

	void removeZone(QStandardItem * zoneItem);
	void removeDefinition(QStandardItem * zoneItem);

	void changeZoneName(QStandardItem * zoneNameItem);
	void changeDefinitionTime(QStandardItem * definitionStartItem, bool start);

	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & space) const;
	QList<QStandardItem*> buildZone(const fmp::Zone::Ptr & zone) const;
	QList<QStandardItem*> buildDefinition(const fmp::Zone::Definition::Ptr & pdefinition) const;

	QStandardItemModel  * d_spaceModel;
	QStandardItemModel  * d_fullFrameModel;
	QStandardItemModel  * d_zoneModel;
	fmp::Experiment::Ptr  d_experiment;
	fmp::Space::Ptr       d_selectedSpace;
};
