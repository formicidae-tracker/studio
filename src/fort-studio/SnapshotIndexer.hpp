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

public slots:
	size_t start();
	void cancel();

signals:
	void resultReady(const QVector<Snapshot::ConstPtr>&, size_t done);

private:
	struct ImageToProcess {
		std::filesystem::path Basedir,Datadir,Path;
		uint64_t Frame;
		uint32_t * Filter;
		QVector<Snapshot::ConstPtr> Results;
	};

	void Process(ImageToProcess & tp);

	std::filesystem::path d_basedir;
	std::filesystem::path d_datadir;

	fort::myrmidon::priv::Experiment::TagFamily d_familyValue;

	std::shared_ptr<apriltag_family_t>   d_family;
	std::shared_ptr<apriltag_detector_t> d_detector;
	std::mutex                           d_mutex;
	bool                                 d_quit;

	QVector<ImageToProcess>    d_toProcess;
	QFuture<void>              d_future;

};
