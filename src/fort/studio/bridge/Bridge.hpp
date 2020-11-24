#pragma once

#include <QObject>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>

class ExperimentBridge;

class Bridge : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool modified
	           READ isModified
	           NOTIFY modified)
	Q_PROPERTY(bool active
	           READ isActive
	           NOTIFY activated)
public:
	Bridge(QObject * parent);
	virtual ~Bridge();
	bool isModified() const;

	virtual bool isActive() const = 0;

signals:
	void modified(bool);
	void activated(bool);

public slots:
	void setModified(bool);

private:
	bool d_modified;
};

class GlobalBridge : public Bridge {
	Q_OBJECT
public:
	GlobalBridge(QObject * parent);
	virtual ~GlobalBridge();

	bool isActive() const final override;

	virtual void initialize(ExperimentBridge * experiment) = 0;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

protected:
	fmp::Experiment::Ptr d_experiment;

	virtual void setUpExperiment() = 0;
	virtual void tearDownExperiment() = 0;


};
