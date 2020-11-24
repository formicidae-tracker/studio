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
	virtual ~SelectedAntBridge();

	const fmp::Ant::TypedCapsuleList & capsules() const;

	fmp::AntID selectedID() const;

	std::vector<fmp::Identification::ConstPtr> identifications() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

public slots:
	void setAnt(const fmp::Ant::Ptr & ant);

	void addCapsule(fmp::AntShapeTypeID typeID,const fmp::CapsulePtr & capsule);
	void clearCapsules();

	void cloneShape(bool scaleToSize, bool overwriteShape);

private:

	IdentifierBridge             * d_identifier;

	fmp::Ant::Ptr                  d_ant;
	fmp::Experiment::Ptr           d_experiment;
};
