#pragma once

#include <QObject>
#include <QFutureWatcher>

#include "Snapshot.hpp"

#include <apriltag/apriltag.h>

#include <myrmidon/priv/Experiment.hpp>

namespace fort{
namespace myrmidon{
namespace pb{
class TagCloseUp;
}
}
}

class SnapshotIndexer : public QObject {
	Q_OBJECT
public:
	SnapshotIndexer(const fort::myrmidon::priv::TrackingDataDirectory::ConstPtr & tdd,
	                const fs::path & basedir,
	                fort::tags::Family family,
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
		fs::path Basedir,RelativeImagePath;
		fort::myrmidon::priv::RawFrameConstPtr Frame;
		fort::myrmidon::priv::TagID * Filter;
		QVector<Snapshot::ConstPtr> Results;
	};

	void Process(ImageToProcess & tp);

	void LoadCache();
	void SaveCache();
	Snapshot::ConstPtr LoadSnapshot(const fort::myrmidon::pb::TagCloseUp & pb);
	void SaveSnapshot(fort::myrmidon::pb::TagCloseUp & pb, const Snapshot::ConstPtr & s);


	fort::myrmidon::priv::TrackingDataDirectory::ConstPtr d_tdd;
	fs::path                                    d_basedir;

	fort::tags::Family d_familyValue;

	std::shared_ptr<apriltag_family_t>   d_family;
	std::shared_ptr<apriltag_detector_t> d_detector;
	std::mutex                           d_mutex;
	bool                                 d_quit;

	QVector<ImageToProcess>    d_toProcess;
	QFuture<void>              d_future;

	std::map<fs::path,QVector<Snapshot::ConstPtr> > d_cache;

};
