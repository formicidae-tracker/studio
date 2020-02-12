#pragma once

#include <QStandardItemModel>

#include "IdentifierBridge.hpp"
#include "SelectedIdentificationBridge.hpp"

#include <myrmidon/priv/Ant.hpp>


class SelectedAntBridge : public QObject {
	Q_OBJECT
public:
	SelectedAntBridge(QObject * parent);

	QAbstractItemModel * identificationModel() const;
	QAbstractItemModel * shapeModel() const;

public slots:
	void setAnt(const fmp::Ant::Ptr & ant);

	void onIdentificationModified(const fmp::Identification::ConstPtr &);

signals:
	void activeStateChanged(bool);

private:
	void rebuildIdentificationModel();

	QStandardItemModel * d_identificationModel;
	QStandardItemModel * d_shapeModel;
	fmp::Ant::Ptr        d_ant;
};
