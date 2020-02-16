#pragma once

#include <myrmidon/priv/Identification.hpp>

#include "Bridge.hpp"

#include "MyrmidonTypes.hpp"

class SelectedIdentificationBridge : public Bridge  {
	Q_OBJECT
	Q_PROPERTY(fm::Time::ConstPtr start
	           READ start WRITE setStart
	           NOTIFY startModified);
	Q_PROPERTY(fm::Time::ConstPtr end
	           READ end WRITE setEnd
	           NOTIFY endModified);

public:
	SelectedIdentificationBridge(QObject * parent);

	bool isActive() const override;

	void setIdentification(const fmp::Identification::Ptr & identification);

	fm::Time::ConstPtr start() const;
	fm::Time::ConstPtr end() const;

public slots:
	void setStart(const fm::Time::ConstPtr & );
	void setEnd(const fm::Time::ConstPtr & );

signals:

	void startModified(const fm::Time::ConstPtr &);
	void endModified(const fm::Time::ConstPtr &);

	void identificationModified(const fmp::Identification::ConstPtr & identification);

protected:
	fmp::Identification::Ptr d_identification;
};
