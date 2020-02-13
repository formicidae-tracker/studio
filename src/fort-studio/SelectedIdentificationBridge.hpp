#pragma once

#include <QObject>

#include <myrmidon/priv/Identification.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fm::Time::ConstPtr)

class SelectedIdentificationBridge : public QObject {
	Q_OBJECT
	Q_PROPERTY(fm::Time::ConstPtr start
	           READ start WRITE setStart
	           NOTIFY startModified);
	Q_PROPERTY(fm::Time::ConstPtr end
	           READ end WRITE setEnd
	           NOTIFY endModified);

public:
	SelectedIdentificationBridge(QObject * parent);

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

	void activated(bool);
protected:
	fmp::Identification::Ptr d_identification;
};
