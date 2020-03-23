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

signals:
	void metadataColumnChanged(const QString & name, quint32 type);
	void metadataColumnRemoved(const QString & name);

public slots:
	void addMetadataColumn(const QString & name, quint32 type);
	void removeMetadataColumn(const QString & name);
private:
	QList<QStandardItem*> buildColumn(const fmp::AntMetadata::Column::Ptr & column);


	fmp::ExperimentPtr   d_experiment;
	QStandardItemModel * d_columnModel;

};
