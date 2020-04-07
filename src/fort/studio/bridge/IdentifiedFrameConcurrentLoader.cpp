#include "IdentifiedFrameConcurrentLoader.hpp"

#include <QtConcurrent>

#include <fort/studio/Format.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/MovieSegment.hpp>
#include <fort/myrmidon/priv/RawFrame.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

IdentifiedFrameConcurrentLoader::IdentifiedFrameConcurrentLoader(QObject * parent)
	: QObject(parent)
	, d_done(0)
	, d_toDo(-1)
	, d_currentLoading(-1) {

}

IdentifiedFrameConcurrentLoader::~IdentifiedFrameConcurrentLoader() {
}


bool IdentifiedFrameConcurrentLoader::isDone() const {
	return d_done == d_toDo;
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


	auto identifier = d_experiment->ConstIdentifier().Compile();
	//Don't emit !!!
	d_toDo = 0;

	d_currentLoading = d_abordFlags.size();
	size_t currentLoading = d_currentLoading;
	d_abordFlags.push_back(std::make_shared<std::atomic<bool>>());
	auto abordFlag = d_abordFlags.back();
	abordFlag->store(false);

	// frames are single threaded read and loaded in memory
	auto load =
		[tdd,segment,identifier,abordFlag,currentLoading,this]() {
			try {
				auto start = tdd->FrameAt(segment->StartFrame());
				while( abordFlag->load() != true ) {
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

					setProgress(d_done,d_toDo+1);

					auto loadFrame =
						[rawFrame,identifier,segment,this] () -> ConcurrentResult {
							if ( !rawFrame ) {
								return std::make_pair(segment->EndMovieFrame()+1,
								                      fmp::IdentifiedFrame::ConstPtr());
							}
							auto frameID = rawFrame->Frame().FID();
							try {
								auto movieID = segment->ToMovieFrameID(frameID);
								return std::make_pair(movieID,
								                      rawFrame->IdentifyFrom(*identifier));
							} catch( const std::exception & ) {
								return std::make_pair(segment->EndMovieFrame()+1,
								                      fmp::IdentifiedFrame::ConstPtr());
							}
						};

					QFuture<ConcurrentResult> future = QtConcurrent::run(loadFrame);
					auto watcher = new QFutureWatcher<ConcurrentResult>();
					connect(watcher,
					        &QFutureWatcher<ConcurrentResult>::finished,
					        this,
					        [watcher,currentLoading,segment,this]() {
						        auto res = watcher->result();
						        watcher->deleteLater();

						        if ( currentLoading != this->d_currentLoading ) {
							        return;
						        }

						        setProgress(d_done+1,d_toDo);

						        if ( res.first == segment->EndMovieFrame()+1 ) {
							        return;
						        }

						        d_frames.insert(res.first,res.second);
					        },
					        Qt::QueuedConnection);
					watcher->setFuture(future);
					++start;
				}
			} catch ( const std::exception & e) {
				qCritical() << "Could not extract tracking data for "
				            << ToQString(segment->URI())
				            << ": " << e.what();
				setProgress(d_toDo,d_toDo);
				return;
			}
		};

	QtConcurrent::run(QThreadPool::globalInstance(),load);
}

void IdentifiedFrameConcurrentLoader::clear() {
	abordCurrent();
	d_frames.clear();
}


void IdentifiedFrameConcurrentLoader::abordCurrent() {
	if ( d_abordFlags.empty() == true || d_currentLoading != d_abordFlags.size()-1 ) {
		return;
	}
	d_abordFlags.back()->store(true);
	d_currentLoading = -1;
}


void IdentifiedFrameConcurrentLoader::setProgress(int doneValue,int toDo) {
	if ( d_done == doneValue && d_toDo == toDo ) {
		return;
	}
	bool doneState = isDone();
	d_done = doneValue;
	d_toDo = toDo;
	emit progressChanged(d_done,d_toDo);
	if ( doneState != isDone() ) {
		emit done(isDone());
	}
}
