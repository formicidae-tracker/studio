#include "IdentifiedFrameLoader.hpp"

#include <QtConcurrent>

#include <fort-studio/Format.hpp>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Identifier.hpp>
#include <myrmidon/priv/MovieSegment.hpp>
#include <myrmidon/priv/RawFrame.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

IdentifiedFrameLoader::IdentifiedFrameLoader(QObject * parent)
	: QObject(parent)
	, d_done(true)
	, d_futureWatcher(new QFutureWatcher<MappedResult>(this)) {
	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::finished,
	        this,
	        &IdentifiedFrameLoader::onFinished);

	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::resultReadyAt,
	        this,
	        &IdentifiedFrameLoader::onResultReadyAt,
	        Qt::QueuedConnection);
}

IdentifiedFrameLoader::~IdentifiedFrameLoader() {
}


bool IdentifiedFrameLoader::isDone() const {
	return d_done;
}

void IdentifiedFrameLoader::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	if ( !d_experiment ) {
		clear();
	}
	d_experiment = experiment;
}

const fmp::IdentifiedFrame::ConstPtr &
IdentifiedFrameLoader::FrameAt(fmp::MovieFrameID movieID) const {
	static fmp::IdentifiedFrame::ConstPtr empty;
	auto fi = d_frames.find(movieID);
	if ( fi == d_frames.cend() ) {
		return empty;
	}
	return fi.value();
}


struct  IdentifiedFrameComputer {
	typedef IdentifiedFrameLoader::MappedResult result_type;

	IdentifiedFrameComputer(const fmp::IdentifierIF::ConstPtr & identifier,
	                        const fmp::MovieSegment::ConstPtr & segment,
	                        const fmp::TrackingDataDirectory::const_iterator & start)
		: d_identifier(identifier)
		, d_segment(segment)
		, d_iterator(start) {
	}

	IdentifiedFrameLoader::MappedResult operator()(fmp::FrameID frameID) {
		auto rawFrame = *d_iterator;
		while ( !rawFrame == false  && rawFrame->Frame().FID() < frameID ) {
			++d_iterator;
			rawFrame = * d_iterator;
		}
		auto movieID = d_segment->ToMovieFrameID(frameID);

		if ( !rawFrame || rawFrame->Frame().FID() > frameID ) {
			return std::make_pair(movieID,fmp::IdentifiedFrame::ConstPtr());
		}

		return std::make_pair(movieID,
		                      rawFrame->IdentifyFrom(*d_identifier));
	}

private:
	fmp::IdentifierIF::ConstPtr                d_identifier;
	fmp::MovieSegment::ConstPtr                d_segment;
	fmp::TrackingDataDirectory::const_iterator d_iterator;
};


void IdentifiedFrameLoader::loadMovieSegment(const fmp::TrackingDataDirectory::ConstPtr & tdd,
                                             const fmp::MovieSegment::ConstPtr & segment) {
	if ( !d_experiment ) {
		return;
	}

	if ( fs::path(segment->URI()).parent_path().parent_path().generic_string() != tdd->URI() ) {
		qCritical() << "Cannot load frame from " << ToQString(segment->URI())
		            << " from TrackingDataDirectory " << ToQString(tdd->URI());
		return;
	}
	clear();

	QVector<fmp::FrameID> frames;
	for (fmp::FrameID fid = segment->StartFrame(); fid <= segment->EndFrame(); ++fid ) {
		frames.push_back(fid);
	}
	try {
		auto start = tdd->FrameAt(segment->StartFrame());
		setDone(false);
		auto identifier = d_experiment->ConstIdentifier().Compile();
		QFuture<MappedResult> future =
			QtConcurrent::mapped(frames,
			                     IdentifiedFrameComputer(identifier,
			                                             segment,
			                                             start));
		d_futureWatcher->setFuture(future);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not start frame identification for" << ToQString(segment->URI())
		            <<": " << e.what();
		return;
	}
}


void IdentifiedFrameLoader::clear() {
	d_futureWatcher->cancel();
	disconnect(d_futureWatcher,
	           &QFutureWatcher<MappedResult>::resultReadyAt,
	           this,
	           &IdentifiedFrameLoader::onResultReadyAt);
	d_futureWatcher->waitForFinished();
	d_futureWatcher->setFuture(QFuture<MappedResult>());
	d_frames.clear();
	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::resultReadyAt,
	        this,
	        &IdentifiedFrameLoader::onResultReadyAt,
	        Qt::QueuedConnection);

}


void IdentifiedFrameLoader::setDone(bool done_) {
	if ( done_ == d_done ) {
		return;
	}
	d_done = done_;
	emit done(d_done);
}


void IdentifiedFrameLoader::onResultReadyAt(int index) {
	auto res = d_futureWatcher->resultAt(index);
	d_frames.insert(res.first,res.second);
}

void IdentifiedFrameLoader::onFinished() {
	setDone(true);
}
