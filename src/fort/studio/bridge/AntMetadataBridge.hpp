#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes.hpp>

class QStandardItemModel;
class QStandardItem;
class QAbstractItemModel;

class AntMetadataBridge : public Bridge {
	Q_OBJECT

public :
	AntMetadataBridge(QObject * parent = nullptr);
	virtual ~AntMetadataBridge();

	bool isActive() const override;

	void setExperiment(const fmp::ExperimentPtr & experiment);

	QAbstractItemModel * columnModel();
	QAbstractItemModel * dataModel();
	QAbstractItemModel * typeModel();

signals:
	void metadataColumnChanged(const QString & name, quint32 type);
	void metadataColumnRemoved(const QString & name);

public slots:
	void addMetadataColumn(const QString & name, quint32 type);
	void removeMetadataColumn(const QString & name);

	void onAntListModified();

private slots:
	void onColumnItemChanged(QStandardItem * item);
	void onDataItemChanged(QStandardItem * item);
	void rebuildDataModel();

private:
	QList<QStandardItem*> buildColumn(const fmp::AntMetadata::Column::Ptr & column);
	QString findTypeName(fmp::AntMetadata::Type type);
	QVariant findTypeDefaultValue(fmp::AntMetadata::Type type);

	void setupItemFromValue(QStandardItem * item,
	                        const fmp::Ant::ConstPtr & ant,
	                        const fmp::AntMetadata::Column::ConstPtr & column);

	QString textForValue(const fmp::Ant::ConstPtr & ant,
	                     const fmp::AntMetadata::Column::ConstPtr & column);

	fmp::ExperimentPtr   d_experiment;
	QStandardItemModel * d_columnModel;
	QStandardItemModel * d_typeModel;
	QStandardItemModel * d_dataModel;
};
