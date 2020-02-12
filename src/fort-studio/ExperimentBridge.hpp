#pragma once

#include <QObject>

#include <myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class ExperimentBridge : public QObject {
Q_OBJECT
public:
	ExperimentBridge(QObject * parent);

	void setExperiment(const fmp::Experiment::Ptr & experiment);

public:
	QString name() const;
	QString author() const;
	QString comment() const;
	fort::tags::Family tagFamily() const;
	uint8_t threshold() const;
	double tagSize() const;

signals:
	void nameChanged(QString name);
	void authorChanged(QString author);
	void commentChanged(QString comment);
	void tagFamilyChanged(fort::tags::Family f);
	void thresholdChanged(uint8_t value);
	void tagSizeChanged(double value);

	void activeStateChanged(bool);
public slots:

	void setName(const QString & name);
	void setAuthor(const QString & author);
	void setComment(const QString & comment);
	void setThreshold(uint8_t th);
	void setTagSize(double tagSize);
	void setTagFamily(fort::tags::Family tf);

private:
	fmp::Experiment::Ptr d_experiment;
};
