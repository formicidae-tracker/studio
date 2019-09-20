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

	const QString & AbsolutePath() const;

signals:
	void modified(bool);
	void dataDirUpdated(QStringList);
	void antListModified();
	void pathModified(const QString & path);

public slots:
	Error addDataDirectory(const QString & path, QString & result);
	Error removeDataDirectory(const QString & path);
	Error save(const QString & path);
	Error open(const QString & path);
	void  reset();

private:
	fort::myrmidion::priv::Experiment::Ptr d_experiment;
	bool d_modified;
	QString d_absolutePath;

	void markModified(bool modified);
	void setPath(const QString & path);

	Error openAndParseTrackingDataDirectory(const QString & relativePath, const QString & root,
	                                        fort::myrmidion::pb::TrackingDataDirectory & res);
};
