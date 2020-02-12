#pragma once

#include <QStandardItemModel>

#include "IdentifierBridge.hpp"
#include "SelectedIdentificationBridge.hpp"

#include <myrmidon/priv/Ant.hpp>


class SelectedAntBridge : public QObject {
	Q_OBJECT
public:
	SelectedAntBridge(IdentifierBridge * iBridge,SelectedIdentificationBridge * siBridge,QObject * parent);

	QAbstractItemModel * identificationModel() const;
	QAbstractItemModel * shapeModel() const;

public slots:
	void SetAnt(const fmp::Ant::Ptr & ant);

private slots:
	void onIdentificationModified(const fmp::Identification::ConstPtr &);

private:
	void rebuildIdentificationModel();

	QStandardItemModel * d_identificationModel;
	QStandardItemModel * d_shapeModel;
	fmp::Ant::Ptr        d_ant;
};
