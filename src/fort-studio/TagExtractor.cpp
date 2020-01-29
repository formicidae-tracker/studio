#include "TagExtractor.hpp"

#include <QtConcurrent/QtConcurrent>

#include <fort-hermes/FileContext.h>
#include <fort-hermes/Error.h>

TagExtractor::TagExtractor(QObject * parent)
	: QObject(parent) {
}

TagExtractor::~TagExtractor() {
	cancel();
}

void TagExtractor::start(const fort::myrmidon::priv::TrackingDataDirectory & tdd,
                         const fs::path & basedir) {
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_quit = false;
	}
	auto path = tdd.AbsoluteFilePath() / "tracking.0000.hermes";

	d_future = QtConcurrent::run(QThreadPool::globalInstance(),
	                             [this,path]() {
		                             uint64_t frameCount = 0;
		                             try {
			                             fort::hermes::FileContext ctx(path.string());
			                             fort::hermes::FrameReadout ro;
			                             for(;;) {
				                             ctx.Read(&ro);
				                             {
					                             std::lock_guard<std::mutex> lock(d_mutex);
					                             if (d_quit == true) {
						                             break;
					                             }
				                             }
				                             ++frameCount;
				                             for(const auto & t : ro.tags() ) {
					                             if (d_tags.count(t.id()) == 0 ) {
						                             d_tags[t.id()] = 0;
					                             }
					                             d_tags[t.id()] += 1;
				                             }
			                             }
		                             } catch( const fort::hermes::EndOfFile & ) {
			                             d_results.reserve(d_tags.size());
			                             for(auto [t,count] : d_tags ) {
				                             double ratio= (double)count /(double)frameCount;
				                             if ( ratio > 0.0005 ) {
					                             d_results.push_back(t);
				                             }
			                             }

			                             emit resultReady(d_results);
		                             } catch ( const std::exception & e ) {
			                             std::cerr << "Could not extract all tags from "
			                                       << path << ": " << e.what()
			                                       << std::endl;
		                             }
	                             });

}

void TagExtractor::cancel() {
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_quit = true;

	}
	d_future.waitForFinished();
}
