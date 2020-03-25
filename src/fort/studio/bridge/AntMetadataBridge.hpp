#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes.hpp>

class QStandardItemModel;
class QStandardItem;
class QAbstractItemModel;

class AntMetadataBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(quint32 selectedAntID
	           READ selectedAntID
	           NOTIFY selectedAntIDChanged);
public :
	AntMetadataBridge(QObject * parent = nullptr);
	virtual ~AntMetadataBridge();

	bool isActive() const override;

	void setExperiment(const fmp::ExperimentPtr & experiment);

	QAbstractItemModel * columnModel();
	QAbstractItemModel * dataModel();
	QAbstractItemModel * typeModel();
	QAbstractItemModel * timedChangeModel();

	quint32 selectedAntID() const;

signals:
	void metadataColumnChanged(const QString & name, quint32 type);
	void metadataColumnRemoved(const QString & name);

	void selectedAntIDChanged(quint32 );
public slots:
	void addMetadataColumn(const QString & name, quint32 type);
	void removeMetadataColumn(const QString & name);

	void onAntListModified();

	void selectRow(int row);

	void addTimedChange(quint32 antID,const QString & name);

	void removeTimedChange(const QModelIndex & index);

private slots:
	void onColumnItemChanged(QStandardItem * item);
	void onDataItemChanged(QStandardItem * item);
	void onTimedChangeItemChanged(QStandardItem * item);

	void rebuildDataModel();

private:
	QList<QStandardItem*> buildColumn(const fmp::AntMetadata::Column::Ptr & column);
	QList<QStandardItem*> buildTimedChange(const fmp::Ant::Ptr & ant,
	                                       const fmp::AntMetadata::Column::Ptr & name,
	                                       const fm::Time & time,
	                                       const fmp::AntStaticValue & value);
	QString findTypeName(fmp::AntMetadata::Type type);

	void setupItemFromValue(QStandardItem * item,
	                        const fmp::Ant::ConstPtr & ant,
	                        const fmp::AntMetadata::Column::ConstPtr & column);

	QString textForValue(const fmp::Ant::ConstPtr & ant,
	                     const fmp::AntMetadata::Column::ConstPtr & column);

	void setSelectedAntID(quint32 ID);
	fmp::ExperimentPtr   d_experiment;
	QStandardItemModel * d_columnModel;
	QStandardItemModel * d_typeModel;
	QStandardItemModel * d_dataModel;
	QStandardItemModel * d_timedChangeModel;
	quint32              d_selectedAntID;
};
