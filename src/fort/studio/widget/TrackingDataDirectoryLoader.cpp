#include "TrackingDataDirectoryLoader.hpp"

#include <QProgressDialog>
#include <QtConcurrent>
size_t TrackingDataDirectoryLoader::size() const {
	return d_tagCloseUps.size() + d_tagStatistics.size() + d_fullFrames.size();
}

bool TrackingDataDirectoryLoader::isEmpty() const  {
	return size() == 0;
}

void TrackingDataDirectoryLoader::append(const fmp::TrackingDataDirectory::Ptr & tdd) {
	if ( tdd->TagCloseUpsComputed() == false ) {
		auto loaders = tdd->PrepareTagCloseUpsLoaders();
		d_tagCloseUps.insert(d_tagCloseUps.end(),loaders.begin(),loaders.end());
	}

	if ( tdd->TagStatisticsComputed() == false ) {
		auto loaders = tdd->PrepareTagStatisticsLoaders();
		d_tagStatistics.insert(d_tagStatistics.end(),loaders.begin(),loaders.end());
	}

	if ( tdd->FullFramesComputed() == false ) {
		auto loaders = tdd->PrepareFullFramesLoaders();
		d_fullFrames.insert(d_fullFrames.end(),loaders.begin(),loaders.end());
	}
}

void TrackingDataDirectoryLoader::initDialog(QWidget * parent) {
	if (parent == nullptr ) {
		d_dialog = nullptr;
	} else {
		d_dialog = new QProgressDialog("data",QString(),0,size(),parent);
		d_dialog->setWindowTitle(QObject::tr("Computing tracking directory data"));
		d_dialog->setMinimumDuration(250);
		d_dialog->setAutoClose(false);
		d_dialog->setMinimumSize(QSize(400,40));

		d_dialog->setWindowModality(Qt::ApplicationModal);
		d_dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::FramelessWindowHint);

		connect(this,
		        &TrackingDataDirectoryLoader::valueChanged,
		        d_dialog,
		        &QProgressDialog::setValue,
		        Qt::QueuedConnection);
	}
}

TrackingDataDirectoryLoader::TrackingDataDirectoryLoader(const std::vector<fmp::TrackingDataDirectory::Ptr> & tdds,
                                                         QWidget * parent)
	: d_dialog(nullptr) {

	for ( const auto & tdd : tdds ) {
		append(tdd);
	}
	if ( isEmpty() == false ) {
		initDialog(parent);
	}
}

TrackingDataDirectoryLoader::~TrackingDataDirectoryLoader(){
	if ( d_dialog != nullptr ) {
		delete d_dialog;
		d_dialog = nullptr;
	}
}

void TrackingDataDirectoryLoader::load(const std::vector<fmp::TrackingDataDirectory::Loader> & loaders,
                                       const QString & caption) {
	std::atomic<int> counts;
	counts.store(0);

	if ( d_dialog != nullptr ) {
		d_dialog->setLabelText(caption);
		d_dialog->setRange(0,loaders.size());
		d_dialog->setValue(0);
		d_dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::FramelessWindowHint);
		d_dialog->adjustSize();
	}

	QFutureWatcher<void> watcher;
	QEventLoop loop;

	connect(&watcher,&QFutureWatcher<void>::finished,
	        &loop,&QEventLoop::quit);

	watcher.setFuture(QtConcurrent::map(loaders,
	                                    [&counts,this](const fmp::TrackingDataDirectory::Loader & l) {
		                                    try {
			                                    l();
		                                    } catch( const std::exception & e) {
			                                    qWarning() << e.what();
		                                    }
		                                    int done = counts.fetch_add(1)+1;
		                                    emit valueChanged(done);
	                                    }));
	loop.exec();
}



void TrackingDataDirectoryLoader::loadAll() {
	load(d_tagCloseUps,tr("Computing tag's close-up..."));
	load(d_tagStatistics,tr("Computing tag's statistics..."));
	load(d_fullFrames,tr("Interpolating missing full frames..."));
}


void TrackingDataDirectoryLoader::EnsureLoaded(const std::vector<fmp::TrackingDataDirectory::Ptr> & tdds,
                                               QWidget * parent) {


	TrackingDataDirectoryLoader loader(tdds,parent);
	loader.loadAll();
}
