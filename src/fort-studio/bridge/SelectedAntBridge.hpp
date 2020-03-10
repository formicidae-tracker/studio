#pragma once

#include <QStandardItemModel>

#include <myrmidon/priv/Ant.hpp>

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

	void setExperiment(const fmp::ExperimentConstPtr & experiment);

	SelectedIdentificationBridge * selectedIdentification() const;

	QAbstractItemModel * identificationModel() const;
	QAbstractItemModel * shapeModel() const;

	fm::Ant::ID selectedID() const;

public slots:
	void setAnt(const fmp::Ant::Ptr & ant);

	void onIdentificationModified(const fmp::Identification::ConstPtr &);

	void selectIdentification(const QModelIndex & index);

	void removeIdentification(const QModelIndex & index);
private:
	void rebuildIdentificationModel();

	IdentifierBridge             * d_identifier;
	QStandardItemModel           * d_identificationModel;
	QStandardItemModel           * d_shapeModel;
	SelectedIdentificationBridge * d_selectedIdentification;
	fmp::Ant::Ptr                  d_ant;
	fmp::ExperimentConstPtr        d_experiment;
};
