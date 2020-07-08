#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>

class SelectedIdentificationBridge : public Bridge  {
	Q_OBJECT
	Q_PROPERTY(bool useGlobalSize
	           READ useGlobalSize
	           WRITE setUseGlobalSize
	           NOTIFY useGlobalSizeChanged)
	Q_PROPERTY(double tagSize
	           READ tagSize
	           WRITE setTagSize
	           NOTIFY tagSizeChanged)
	Q_PROPERTY(fm::Time::ConstPtr start
	           READ start WRITE setStart
	           NOTIFY startModified);
	Q_PROPERTY(fm::Time::ConstPtr end
	           READ end WRITE setEnd
	           NOTIFY endModified);

public:
	SelectedIdentificationBridge(QObject * parent);
	virtual ~SelectedIdentificationBridge();

	bool isActive() const override;

	void setExperiment(const fmp::ExperimentConstPtr & experiment);

	void setIdentification(const fmp::IdentificationPtr & identification);

	fm::Time::ConstPtr start() const;
	fm::Time::ConstPtr end() const;


	bool useGlobalSize() const;
	double tagSize() const;

public slots:
	void setStart(const fm::Time::ConstPtr & );
	void setEnd(const fm::Time::ConstPtr & );

	void setTagSize(double tagSize);
	void setUseGlobalSize(bool useGlobalSize);

signals:

	void useGlobalSizeChanged(bool useGlobalSize);
	void tagSizeChanged(double tagSize);

	void startModified(const fm::Time::ConstPtr &);
	void endModified(const fm::Time::ConstPtr &);

	void identificationModified(const fmp::Identification::ConstPtr & identification);

protected:
	fmp::IdentificationPtr d_identification;
	fmp::ExperimentConstPtr     d_experiment;
};
