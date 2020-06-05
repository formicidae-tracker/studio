#pragma once

#include <QStandardItemModel>

#include <fort/myrmidon/priv/Ant.hpp>

#include "Bridge.hpp"

namespace fmp = fort::myrmidon::priv;
namespace fm = fort::myrmidon;

class SelectedIdentificationBridge;
class IdentifierBridge;

class SelectedAntBridge : public Bridge {
	Q_OBJECT
public:
	SelectedAntBridge(IdentifierBridge * parent);

	bool isActive() const override;

	void setExperiment(const fmp::ExperimentPtr & experiment);

	SelectedIdentificationBridge * selectedIdentification() const;

	QAbstractItemModel * identificationModel() const;

	const fmp::Ant::TypedCapsuleList & capsules() const;

	fmp::AntID selectedID() const;

	std::vector<fmp::Identification::ConstPtr> identifications() const;

public slots:
	void setAnt(const fmp::Ant::Ptr & ant);

	void onIdentificationModified(const fmp::Identification::ConstPtr &);

	void selectIdentification(const QModelIndex & index);

	void removeIdentification(const QModelIndex & index);

	void addCapsule(fmp::AntShapeTypeID typeID,const fmp::CapsulePtr & capsule);
	void clearCapsules();

	void cloneShape(bool scaleToSize, bool overwriteShape);

private:
	void rebuildIdentificationModel();

	IdentifierBridge             * d_identifier;
	QStandardItemModel           * d_identificationModel;
	SelectedIdentificationBridge * d_selectedIdentification;
	fmp::Ant::Ptr                  d_ant;
	fmp::ExperimentPtr             d_experiment;
};
