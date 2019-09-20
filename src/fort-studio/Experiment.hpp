#pragma once


#include <QObject>

#include "Error.hpp"

#include <myrmidion/Experiment.priv.hpp>

class Experiment : public QObject {
	Q_OBJECT
public:
	Experiment( QObject * parent = NULL);
	virtual ~Experiment();

	bool isModified() const;

	const std::vector<fort::myrmidion::priv::Ant::Ptr> & Ants() const;


signals:
	void modified(bool);
	void antListModified();

public slots:
	Error addDataDirectory(const QString & path);
	Error removeDataDirectory(const QString & path);
	Error save(const QString & path);
	Error open(const QString & path);
	void  reset();

private:
	fort::myrmidion::priv::Experiment::Ptr d_experiment;
	bool d_modified;

	void markModified(bool modified);
};
