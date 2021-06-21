#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/Capsule.hpp>
#include <fort/myrmidon/Types.hpp>

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

	const fm::TypedCapsuleList & capsuleForAntID(fm::AntID antID) const;


public slots:
	int addCapsule(fm::AntID antID,
	               fmp::AntShapeTypeID typeID,
	               const fmp::CapsulePtr & capsule);


	void clearCapsule(fm::AntID antID);

	void cloneShape(fm::AntID source,
	                bool scaleToSize,
	                bool overwriteShape);

signals:
	void capsuleCreated(quint32 antID,
	                    quint32 index,
	                    quint32 typeID,
	                    const fm::Capsule::Ptr & capsule);

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

	void countAnt(fm::AntID antID, bool sendSignals);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	AntGlobalModel * d_model;
	std::map<fmp::AntShapeTypeID,int> d_columnIndex;
};
