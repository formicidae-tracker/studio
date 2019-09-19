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


signals:
	void modified(bool);
	void error(QString);

public slots:
	Error addDataDirectory(const QString & path);
	Error save(const QString & path);
private:
	fort::myrmidion::priv::Experiment::Ptr d_experiment;
	bool d_modified;

	void markModified(bool modified);
};
