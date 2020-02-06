#pragma once


#include <QAbstractItemModel>

#include <myrmidon/priv/Zone.hpp>


class ZoneModel : public QAbstractItemModel {
	Q_OBJECT
public:
	ZoneModel(QObject * parent);


	QVariant data(const QModelIndex & index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex & index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
	                  int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex & index) const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
private:


	fort::myrmidon::priv::Zone::Group::Ptr  d_group;


};
