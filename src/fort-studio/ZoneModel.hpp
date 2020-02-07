#pragma once

#include <QObject>
#include <QStandardItemModel>

#include <myrmidon/priv/Zone.hpp>

class QAbstractItemModel;

class ZoneModel : public QObject {
	Q_OBJECT
public:
	ZoneModel(QObject * parent);

	QAbstractItemModel * model();

protected slots:
	void on_model_itemChanged(QStandardItem * item);


private:
	enum ObjectType {
	                 ZONE_TYPE = 0,
	                 TDD_TYPE  = 1,
	};

	QList<QStandardItem*> BuildTDD(const fort::myrmidon::priv::TrackingDataDirectoryConstPtr & tdd);
	QList<QStandardItem*> BuildZone(const fort::myrmidon::priv::Zone::Ptr & z);

	void BuildAll(const std::vector<fort::myrmidon::priv::Zone::Ptr> & zones);

	fort::myrmidon::priv::Zone::Group::Ptr    d_group;
	QStandardItemModel                      * d_model;

};
