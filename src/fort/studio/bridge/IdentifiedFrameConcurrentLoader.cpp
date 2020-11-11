#include "IdentifiedFrameConcurrentLoader.hpp"

#include <QtConcurrent>

#include <fort/studio/Format.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/MovieSegment.hpp>
#include <fort/myrmidon/priv/RawFrame.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/CollisionSolver.hpp>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>


#ifdef NDEBUG
#define FORT_STUDIO_CONC_LOADER_NDEBUG 1
#endif
#ifndef FORT_STUDIO_CONC_LOADER_NDEBUG
#include <mutex>
std::mutex clDebugMutex;
#define CONC_LOADER_DEBUG(statements) do{ \
		std::lock_guard<std::mutex> dlock(clDebugMutex); \
		statements; \
	}while(0)
#else
#define CONC_LOADER_DEBUG(statements)
#endif


IdentifiedFrameConcurrentLoader::IdentifiedFrameConcurrentLoader(QObject * parent)
	: QObject(parent)
	, d_done(0)
	, d_toDo(-1)
	, d_currentLoadingID(-1)
	, d_connectionType(Qt::QueuedConnection) {
	qRegisterMetaType<fmp::Experiment::ConstPtr>();
}

IdentifiedFrameConcurrentLoader::~IdentifiedFrameConcurrentLoader() {
	if ( !d_abordFlag == false ) {
		d_abordFlag->store(true);
	}
}


bool IdentifiedFrameConcurrentLoader::isDone() const {
	return d_done == d_toDo;
}

void IdentifiedFrameConcurrentLoader::moveToThread(QThread * thread) {
	if ( thread != QObject::thread() ) {
		d_connectionType = Qt::BlockingQueuedConnection;
	}
	QObject::moveToThread(thread);
}


void IdentifiedFrameConcurrentLoader::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	metaObject()->invokeMethod(this,"setExperimentUnsafe",d_connectionType,
	                           Q_ARG(fmp::Experiment::ConstPtr,experiment));
}


void IdentifiedFrameConcurrentLoader::setExperimentUnsafe(fmp::Experiment::ConstPtr experiment) {
	if ( !d_experiment ) {
		clear();
	}
	d_experiment = experiment;
}

const fm::IdentifiedFrame::ConstPtr &
IdentifiedFrameConcurrentLoader::frameAt(fmp::MovieFrameID movieID) const {
	static fm::IdentifiedFrame::ConstPtr empty;
	auto fi = d_frames.find(movieID+1);
	if ( fi == d_frames.cend() ) {
		return empty;
	}
	return fi->second;
}

const fm::CollisionFrame::ConstPtr &
IdentifiedFrameConcurrentLoader::collisionAt(fmp::MovieFrameID movieID) const {
	static fm::CollisionFrame::ConstPtr empty;
	auto fi = d_collisions.find(movieID+1);
	if ( fi == d_collisions.cend() ) {
		return empty;
	}
	return fi->second;
}

void IdentifiedFrameConcurrentLoader::loadMovieSegment(quint32 spaceID,
                                                       const fmp::TrackingDataDirectory::Ptr & tdd,
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
	auto identifier = d_experiment->CIdentifier().Compile();
	auto solver = d_experiment->CompileCollisionSolver();

	size_t currentLoadingID = ++d_currentLoadingID;

	d_abordFlag = std::make_shared<std::atomic<bool>>();
	auto abordFlag = d_abordFlag;
	abordFlag->store(false);

	int maxThreadCount = QThreadPool::globalInstance()->maxThreadCount();
	if ( maxThreadCount < 2 ) {
		qWarning() << "Increases the work thread to at least 2 from " << maxThreadCount;
		maxThreadCount = 2;
		// avoids deadlock on the global instance !!!
		QThreadPool::globalInstance()->setMaxThreadCount(maxThreadCount);
	}
	CONC_LOADER_DEBUG({
			std::cerr << "Setting nbFrames to " << segment->EndFrame() - segment->StartFrame() + 1 << std::endl;
			std::cerr << "Segment:[" << segment->StartFrame() << ";" << segment->EndFrame() << "]" << std::endl;
			std::cerr << "TDD:[" << tdd->StartFrame() << ";" << tdd->EndFrame() << "]" << std::endl;
		});
	setProgress(0, segment->EndFrame() - segment->StartFrame() + 1);
	// even if we take one of the thread to populate tge other, we
	// make sure there could be one in the queue, but no more, to be
	// able to abord computation rapidly..
	auto sem = std::make_shared<QSemaphore>(maxThreadCount);
	// frames are single threaded read and loaded in memory, and we
	// spawn instance to compute them.
	auto load =
		[tdd,segment,identifier,solver,abordFlag,currentLoadingID,sem,spaceID,this]() {
			try {
				auto start = tdd->FrameAt(segment->StartFrame());
				auto lastFrame = segment->StartFrame() - 1;
				while( abordFlag->load() != true ) {
					if ( start == tdd->end() ) {
						break;
					}

					auto rawFrame = *start;
					// We may jump frame number if there is no data,
					// it may be the last frame on the MovieSegment
					// that is jumped.
					if ( !rawFrame  || rawFrame->Frame().FrameID() > segment->EndFrame() ) {
						CONC_LOADER_DEBUG(std::cerr << "marking " <<segment->EndFrame() - lastFrame << " done" << std::endl);
						//mark all jumped frame done
						this->metaObject()->invokeMethod(this,"addDone",Qt::QueuedConnection,
						                                 Q_ARG(int,segment->EndFrame() - lastFrame));
						break;
					}
					auto frameID = rawFrame->Frame().FrameID();
					//we mark all jumped frame done
					CONC_LOADER_DEBUG(std::cerr << "advanced " << frameID - lastFrame << std::endl);
					if ( (frameID - lastFrame) > 1 ) {
						this->metaObject()->invokeMethod(this,"addDone",Qt::QueuedConnection,
						                                 Q_ARG(int,frameID - lastFrame - 1));
					}
					lastFrame = frameID;

					auto loadFrame =
						[rawFrame,identifier,solver,spaceID,segment,frameID,this] () -> ConcurrentResult {
							CONC_LOADER_DEBUG(std::cerr << "Processing " << rawFrame->Frame().FID() << std::endl);
							try {
								auto movieID = segment->ToMovieFrameID(frameID);
								auto identified = rawFrame->IdentifyFrom(*identifier,spaceID);
								auto collisions = solver->ComputeCollisions(identified);
								return std::make_tuple(movieID,
								                       identified,
								                       collisions);
							} catch( const std::exception & ) {
								return std::make_tuple(segment->EndMovieFrame()+1,
								                       fm::IdentifiedFrame::ConstPtr(),
								                       fm::CollisionFrame::ConstPtr());
							}
						};
					CONC_LOADER_DEBUG(std::cerr << "Spawning " << frameID << std::endl);
					sem->acquire(1);
					QFuture<ConcurrentResult> future = QtConcurrent::run(loadFrame);
					auto watcher = new QFutureWatcher<ConcurrentResult>();
					watcher->moveToThread(this->thread());
					connect(watcher,
					        &QFutureWatcher<ConcurrentResult>::finished,
					        this,
					        [watcher,currentLoadingID,segment,sem,frameID,this]() {
						        CONC_LOADER_DEBUG({
								        std::cerr << "Received " << frameID << " status " << d_done << "/" << d_toDo << std::endl;
								        std::cerr << "Wanted Thread:" << this->thread() << " current: " << QThread::currentThread() <<  std::endl;
							        });
						        auto res = watcher->result();
						        watcher->deleteLater();
						        sem->release(1);

						        if ( currentLoadingID != this->d_currentLoadingID ) {
							        CONC_LOADER_DEBUG(std::cerr << "Unexpected loadingID " << currentLoadingID << " (expected:" << this->d_currentLoadingID << std::endl);
							        // outdated computation, we ignore it
							        return;
						        }

						        addDone(1);

						        if ( std::get<0>(res) == segment->EndMovieFrame()+1 ) {
							        // no result for that computation
							        return;
						        }

						        d_frames.insert(std::make_pair(std::get<0>(res)+1,std::get<1>(res)));
						        d_collisions.insert(std::make_pair(std::get<0>(res)+1,std::get<2>(res)));
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
	d_collisions.clear();
}


void IdentifiedFrameConcurrentLoader::abordCurrent() {
	if ( !d_abordFlag ) {
		return;
	}
	d_abordFlag->store(true);
	d_abordFlag.reset();
}

void IdentifiedFrameConcurrentLoader::setProgress(int doneValue,int toDo) {
	CONC_LOADER_DEBUG({
			std::cerr << "[setProgress]: wantedThread: " << this->thread() << " current: " << QThread::currentThread() <<  std::endl;
			std::cerr << "[setProgress]: current:" << d_done << "/" << d_toDo << " wants:" << doneValue << "/" << toDo << std::endl;
		});

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


void IdentifiedFrameConcurrentLoader::addDone(int done) {
	setProgress(d_done + done,d_toDo);
}

quint64 IdentifiedFrameConcurrentLoader::findAnt(quint32 antID,
                                                 quint64 frameID,
                                                 int direction) {
	auto fi = d_frames.find(frameID + 1);
	if ( d_done == false || fi == d_frames.end() ) {
		return std::numeric_limits<quint64>::max();
	}


	if ( direction > 0 ) {
		for ( ; fi != d_frames.end(); ++fi ) {
			if( fi->second->Contains(antID) == true ) {
				return fi->first - 1;
			}
		}
		return std::numeric_limits<quint64>::max();
	}


	for ( ; fi != d_frames.begin(); --fi) {
		if( fi->second->Contains(antID) == true ) {
			return fi->first - 1;
		}
	}
	if ( fi->second->Contains(antID) == true ) {
		return fi->first - 1;
	}
	return std::numeric_limits<quint64>::max();
}
