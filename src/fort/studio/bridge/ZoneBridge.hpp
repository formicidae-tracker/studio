#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

#include <QList>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;

class ZoneDefinitionBridge : public Bridge {
	Q_OBJECT
public:
	ZoneDefinitionBridge(const fmp::Zone::ConstPtr & zone,
	                     const fmp::Zone::Definition::Ptr & ptr);
	virtual ~ZoneDefinitionBridge();
	bool isActive() const override;

	const fmp::Zone::Geometry & geometry() const;

	void setGeometry(const std::vector<fmp::Shape::ConstPtr> & shapes);

	const fmp::Zone & zone() const;
signals:
	void countUpdated(int i);

private:
	fmp::Zone::Definition::Ptr d_definition;
	fmp::Zone::ConstPtr        d_zone;
};


class ZoneBridge : public Bridge {
	Q_OBJECT
public :
	ZoneBridge(QObject * parent);
	virtual ~ZoneBridge();
	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

	QAbstractItemModel * spaceModel() const;
	QAbstractItemModel * fullFrameModel() const;

	void selectTime(const fm::Time & time);

	bool canAddItemAt(const QModelIndex & index);
	bool canRemoveItemAt(const QModelIndex & index);

	struct FullFrame {
		fmp::FrameReference Reference;
		QString             AbsoluteFilePath;
	};

	std::pair<bool,FullFrame> fullFrameAtIndex(const QModelIndex & index) const;


signals:
	void newZoneDefinitionBridge(QList<ZoneDefinitionBridge*>);

	void definitionUpdated();
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

	void rebuildChildBridges();

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
	fmp::Experiment::Ptr  d_experiment;
	fmp::Space::Ptr       d_selectedSpace;
	fm::Time::ConstPtr    d_selectedTime;

	std::vector<std::shared_ptr<ZoneDefinitionBridge>> d_childBridges;
};
