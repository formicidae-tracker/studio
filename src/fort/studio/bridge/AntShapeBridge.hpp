#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/Capsule.hpp>

class QAbstractItemModel;
class AntGlobalModel;
class QStandardItem;

class AntShapeBridge : public GlobalBridge {
	Q_OBJECT
public:
	explicit AntShapeBridge(QObject * parent = nullptr);
	virtual ~AntShapeBridge();

	QAbstractItemModel * model() const;

	void initialize(ExperimentBridge * experiment) override;

public slots:
	int addCapsule(fm::Ant::ID antID,
	               fmp::AntShapeTypeID typeID,
	               const fmp::CapsulePtr & capsule);


	void clearCapsule(fm::Ant::ID antID);

	void cloneShape(fm::Ant::ID source,
	                bool scaleToSize,
	                bool overwriteShape);

signals:
	void capsuleCreated(quint32 antID,
	                    quint32 index,
	                    quint32 typeID,
	                    const fmp::Capsule::ConstPtr & capsule);

	void capsuleCleared(quint32 ant);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;


protected slots:
	void onTypeModified(quint32 shapeTypeID,QString name);
	void onTypeDeleted(quint32 shapeTypeID);

	void onAntCreated(quint32 antID);
	void onAntDeleted(quint32 antID);
private:
	void rebuildColumnIndex();

	void countAnt(fm::Ant::ID antID, bool sendSignals);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	AntGlobalModel * d_model;
	std::map<fmp::AntShapeTypeID,int> d_columnIndex;
};
