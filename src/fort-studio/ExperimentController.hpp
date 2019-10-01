#pragma once


#include <QObject>

#include "Error.hpp"

#include <myrmidon/priv/Experiment.hpp>



class ExperimentController : public QObject {
	Q_OBJECT
public:
	virtual ~ExperimentController();

	const fort::myrmidon::priv::Experiment & experiment() const;

	bool isModified() const;

	static ExperimentController * open(const QString & path, QObject * parent, Error & error);
	static ExperimentController * create(const QString & path, QObject * parent, Error & error);


signals:
	void modified(bool);
	void dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & );
	void antListModified(const fort::myrmidon::priv::Experiment::AntByID & );
public slots:

	Error addDataDirectory(const QString & path);
	Error removeDataDirectory(const QString & path);
	Error save(const QString & path);
	void  setModified(bool modified);

	void createAnt();
	Error removeAnt(fort::myrmidon::Ant::ID ID);

	void setName(const QString & name);
	void setAuthor(const QString & author);
	void setComment(const QString & comment);
	void setTagFamily(fort::myrmidon::priv::Experiment::TagFamily tf);

private:
	fort::myrmidon::priv::Experiment::Ptr d_experiment;
	bool d_modified;

	ExperimentController(fort::myrmidon::priv::Experiment::Ptr & experiment,
	                     QObject * parent = NULL);

};
