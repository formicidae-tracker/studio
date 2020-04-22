#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes.hpp>

#include <QList>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;

class AntShapeTypeBridge : public Bridge {
	Q_OBJECT
public :
	explicit AntShapeTypeBridge(QObject * parent = nullptr);
	virtual ~AntShapeTypeBridge();

	typedef std::map<fmp::AntShapeType::ID,fmp::AntShapeType::ConstPtr> AntShapeTypesByID;

	void setExperiment(const fmp::ExperimentPtr & experiment);

	QAbstractItemModel * shapeModel() const;

	bool isActive() const override;

	AntShapeTypesByID types() const;

public slots:
	void addType(const QString & name);
	void deleteType(quint32 typeID);

signals:
	void typeModified(quint32,QString);
	void typeDeleted(quint32);

private slots:
	void onTypeItemChanged(QStandardItem * item);

private:
	QList<QStandardItem*> buildTypeItem(const fmp::AntShapeTypePtr & shapeType);

	QStandardItemModel * d_model;
	fmp::ExperimentPtr   d_experiment;
};
