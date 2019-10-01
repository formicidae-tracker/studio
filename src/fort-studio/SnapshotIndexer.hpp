#pragma once

#include <QObject>
#include <QFutureWatcher>

#include "Snapshot.hpp"

#include "apriltag.h"

#include <myrmidon/priv/Experiment.hpp>


class SnapshotIndexer : public QObject {
	Q_OBJECT
public:
	SnapshotIndexer(const std::filesystem::path & datadir,
	                const std::filesystem::path & basedir,
	                fort::myrmidon::priv::Experiment::TagFamily family,
	                uint8_t threshold,
	                QObject * parent = NULL);
	virtual ~SnapshotIndexer();

signals:
	void newSnapshot(Snapshot::ConstPtr);

public slots:
	void start();
	void cancel();

private slots:
	void onResultReady(int index);

private:
	struct ImageToProcess {
		std::filesystem::path Basedir,Datadir,Path;
		uint64_t Frame;
		uint32_t * Filter;
		std::vector<Snapshot::ConstPtr> Results;
	};





	std::filesystem::path d_basedir;
	std::filesystem::path d_datadir;

	fort::myrmidon::priv::Experiment::TagFamily d_familyValue;

	std::shared_ptr<apriltag_family_t>   d_family;
	std::shared_ptr<apriltag_detector_t> d_detector;
	std::mutex                           d_detectorMutex;

	QVector<ImageToProcess>                d_toProcess;
	QFutureWatcher<void>       d_futureWatcher;

};
