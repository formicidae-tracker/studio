#include "IdentifiedFrameConcurrentLoader.hpp"

#include <QtConcurrent>

#include <fort-studio/Format.hpp>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Identifier.hpp>
#include <myrmidon/priv/MovieSegment.hpp>
#include <myrmidon/priv/RawFrame.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

IdentifiedFrameConcurrentLoader::IdentifiedFrameConcurrentLoader(QObject * parent)
	: QObject(parent)
	, d_done(true)
	, d_futureWatcher(new QFutureWatcher<MappedResult>(this)) {
	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::finished,
	        this,
	        &IdentifiedFrameConcurrentLoader::onFinished);

	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::resultReadyAt,
	        this,
	        &IdentifiedFrameConcurrentLoader::onResultReadyAt,
	        Qt::QueuedConnection);
}

IdentifiedFrameConcurrentLoader::~IdentifiedFrameConcurrentLoader() {
}


bool IdentifiedFrameConcurrentLoader::isDone() const {
	return d_done;
}

void IdentifiedFrameConcurrentLoader::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	if ( !d_experiment ) {
		clear();
	}
	d_experiment = experiment;
}

const fmp::IdentifiedFrame::ConstPtr &
IdentifiedFrameConcurrentLoader::FrameAt(fmp::MovieFrameID movieID) const {
	static fmp::IdentifiedFrame::ConstPtr empty;
	auto fi = d_frames.find(movieID);
	if ( fi == d_frames.cend() ) {
		return empty;
	}
	return fi.value();
}


struct IdentifiedFrameComputer {
	typedef IdentifiedFrameConcurrentLoader::MappedResult result_type;

	IdentifiedFrameComputer(const fmp::IdentifierIF::ConstPtr & identifier,
	                        const fmp::MovieSegment::ConstPtr & segment)
		: d_identifier(identifier)
		, d_segment(segment) {
	}

	IdentifiedFrameConcurrentLoader::MappedResult
	operator()(const fmp::RawFrame::ConstPtr & rawFrame) {
		if ( !rawFrame ) {
			return std::make_pair(d_segment->EndMovieFrame()+1,
			                      fmp::IdentifiedFrame::ConstPtr());
		}
		auto frameID = rawFrame->Frame().FID();
		fmp::MovieFrameID movieID;

		try {
			movieID = d_segment->ToMovieFrameID(frameID);
		} catch ( const std::exception & ) {
			return std::make_pair(d_segment->EndMovieFrame()+1,
			                      fmp::IdentifiedFrame::ConstPtr());
		}

		return std::make_pair(movieID,
		                      rawFrame->IdentifyFrom(*d_identifier));
	}

private:
	fmp::IdentifierIF::ConstPtr d_identifier;
	fmp::MovieSegment::ConstPtr d_segment;
};


void IdentifiedFrameConcurrentLoader::loadMovieSegment(const fmp::TrackingDataDirectory::ConstPtr & tdd,
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

	setDone(false);
	auto identifier = d_experiment->ConstIdentifier().Compile();
	// frames are single threaded read and loaded in memory
	auto load =
		[tdd,segment,identifier,this]() {
			QVector<fmp::RawFrame::ConstPtr> frames;
			frames.reserve(segment->EndFrame() - segment->StartFrame());
			try {
				auto start = tdd->FrameAt(segment->StartFrame());
				while(true) {
					if ( start == tdd->end() ) {
						break;
					}

					auto rawFrame = *start;
					// We may jump frame number if there is no data,
					// it may be the last frame on the MovieSegment
					// that is jumped.
					if ( !rawFrame || rawFrame->Frame().FID() > segment->EndFrame() ) {
						break;
					}
					frames.push_back(rawFrame);
					++start;
				}
			} catch ( const std::exception & e) {
				qCritical() << "Could not extract tracking data for "
				            << ToQString(segment->URI())
				            << ": " << e.what();
				setDone(true);
				return;
			}
			// for each frame, concurrently Compute each IdentifiedFrame
			auto future =
				QtConcurrent::mapped(frames,
				                     IdentifiedFrameComputer(identifier,
				                                             segment));
			d_futureWatcher->setFuture(future);
		};

	QtConcurrent::run(QThreadPool::globalInstance(),load);
}

void IdentifiedFrameConcurrentLoader::clear() {
	d_futureWatcher->cancel();
	disconnect(d_futureWatcher,
	           &QFutureWatcher<MappedResult>::resultReadyAt,
	           this,
	           &IdentifiedFrameConcurrentLoader::onResultReadyAt);
	d_futureWatcher->waitForFinished();
	d_futureWatcher->setFuture(QFuture<MappedResult>());
	d_frames.clear();
	connect(d_futureWatcher,
	        &QFutureWatcher<MappedResult>::resultReadyAt,
	        this,
	        &IdentifiedFrameConcurrentLoader::onResultReadyAt,
	        Qt::QueuedConnection);

}

void IdentifiedFrameConcurrentLoader::setDone(bool done_) {
	if ( done_ == d_done ) {
		return;
	}
	d_done = done_;
	emit done(d_done);
}


void IdentifiedFrameConcurrentLoader::onResultReadyAt(int index) {
	auto res = d_futureWatcher->resultAt(index);
	if ( !res.second ) {
		return;
	}
	d_frames.insert(res.first,res.second);
}

void IdentifiedFrameConcurrentLoader::onFinished() {
	setDone(true);
}
