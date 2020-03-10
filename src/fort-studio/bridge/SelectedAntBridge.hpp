#pragma once

#include <QStandardItemModel>

#include <myrmidon/priv/Ant.hpp>

#include "IdentifierBridge.hpp"

namespace fmp = fort::myrmidon::priv;
namespace fm = fort::myrmidon;

class SelectedAntBridge : public Bridge {
	Q_OBJECT
public:
	SelectedAntBridge(QObject * parent);

	bool isActive() const override;

	QAbstractItemModel * identificationModel() const;
	QAbstractItemModel * shapeModel() const;

	fm::Ant::ID selectedID() const;

public slots:
	void setAnt(const fmp::Ant::Ptr & ant);

	void onIdentificationModified(const fmp::Identification::ConstPtr &);

private:
	void rebuildIdentificationModel();

	QStandardItemModel * d_identificationModel;
	QStandardItemModel * d_shapeModel;
	fmp::Ant::Ptr        d_ant;
};
