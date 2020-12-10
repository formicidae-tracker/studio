#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes/AntShapeType.hpp>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;

class AntShapeTypeBridge : public GlobalBridge {
	Q_OBJECT
public :
	explicit AntShapeTypeBridge(QObject * parent = nullptr);
	virtual ~AntShapeTypeBridge();

	typedef std::map<fmp::AntShapeType::ID,fmp::AntShapeType::ConstPtr> AntShapeTypesByID;

	QAbstractItemModel * shapeModel() const;

	AntShapeTypesByID types() const;

	void initialize(ExperimentBridge * experiment) override;

public slots:
	void addType(const QString & name);
	void deleteType(quint32 typeID);

signals:
	void typeModified(quint32,QString);
	void typeDeleted(quint32);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:
	void onTypeItemChanged(QStandardItem * item);

private:
	QList<QStandardItem*> buildTypeItem(const fmp::AntShapeTypePtr & shapeType);

	QStandardItemModel * d_model;
};
