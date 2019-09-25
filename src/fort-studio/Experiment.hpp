#pragma once


#include <QObject>

#include "Error.hpp"

#include <myrmidon/priv/Experiment.hpp>

namespace fmp = fort::myrmidon::priv;

class ExperimentController : public QObject {
	Q_OBJECT
public:
	ExperimentController(fmp::Ptr,
	                     QObject * parent = NULL);
	virtual ~Experiment();

	std::unique_ptr<const Experiment> & Experiment() const;

	bool isModified() const;

signals:
	void modified(bool);
	void dataDirUpdated(const fmp::TrackingDataDirectoryByPath & );
	void antListModified(const fmp::AntsByID & );
public slots:

	Error addDataDirectory(const QString & path, QString & result);
	Error removeDataDirectory(const QString & path);
	Error save(const QString & path);

private:
	fort::myrmidon::priv::Experiment::Ptr d_experiment;
	bool d_modified;

	void setModified(bool modified);

	Error openAndParseTrackingDataDirectory(const QString & relativePath, const QString & root,
	                                        fort::myrmidon::pb::TrackingDataDirectory & res);
};
