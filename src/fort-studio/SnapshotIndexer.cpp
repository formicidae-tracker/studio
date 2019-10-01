#include "SnapshotIndexer.hpp"

#include <tag16h5.h>
#include <tag25h9.h>
#include <tag36h11.h>
#include <tagCircle21h7.h>
#include <tagCircle49h12.h>
#include <tagCustom48h12.h>
#include <tagStandard41h12.h>
#include <tagStandard52h13.h>
#include <legacy-tags/tag36ARTag.h>
#include <legacy-tags/tag36h10.h>

#include <cctype>

#include <regex>

#include <QtConcurrent/QtConcurrent>
#include <QImage>


SnapshotIndexer::SnapshotIndexer(const std::filesystem::path & datadir,
                                 const std::filesystem::path & basedir,
                                 fort::myrmidon::priv::Experiment::TagFamily family,
                                 uint8_t threshold,
                                 QObject * parent)
	: QObject(parent)
	, d_basedir(basedir)
	, d_datadir(datadir)
	, d_familyValue(family) {
	connect(&d_futureWatcher,SIGNAL(resultReadyAt(int)),
	        this,SLOT(onResultReady(int)));
	struct FamilyInterface {
		typedef apriltag_family_t* (*Constructor) ();
		typedef void (*Destructor) (apriltag_family_t *);
		Constructor c;
		Destructor  d;
	};
	using namespace fort::myrmidon::priv;
	static std::unordered_map<Experiment::TagFamily,FamilyInterface>  familyFactory = {
		 {Experiment::TagFamily::Tag16h5,{.c = tag16h5_create, .d = tag16h5_destroy}},
		 {Experiment::TagFamily::Tag25h9,{.c =tag25h9_create, .d=tag25h9_destroy}},
		 {Experiment::TagFamily::Tag36h10,{.c =tag36h10_create, .d=tag36h10_destroy}},
		 {Experiment::TagFamily::Tag36h11,{.c =tag36h11_create, .d=tag36h11_destroy}},
		 {Experiment::TagFamily::Tag36ARTag,{.c =tag36ARTag_create, .d=tag36ARTag_destroy}},
		 {Experiment::TagFamily::Circle21h7,{.c =tagCircle21h7_create, .d=tagCircle21h7_destroy}},
		 {Experiment::TagFamily::Circle49h12,{.c =tagCircle49h12_create, .d=tagCircle49h12_destroy}},
		 {Experiment::TagFamily::Custom48h12,{.c =tagCustom48h12_create, .d=tagCustom48h12_destroy}},
		 {Experiment::TagFamily::Standard41h12,{.c =tagStandard41h12_create, .d=tagStandard41h12_destroy}},
		 {Experiment::TagFamily::Standard52h13,{.c =tagStandard52h13_create, .d=tagStandard52h13_destroy}},
	};

	auto f = familyFactory.find(family);
	if ( f == familyFactory.end() ) {
		std::ostringstream os;
		os << "Unsupported family : " << family;
		throw std::runtime_error(os.str());
	}
	d_family = std::shared_ptr<apriltag_family_t>(f->second.c(),f->second.d);
	d_detector = std::shared_ptr<apriltag_detector_t>(apriltag_detector_create(),apriltag_detector_destroy);

	apriltag_detector_add_family(d_detector.get(),d_family.get());
	d_detector->nthreads = 1;
	d_detector->quad_decimate = 1.0;
	d_detector->quad_sigma = 0.0;
	d_detector->refine_edges = 0;
	d_detector->debug = false;
	d_detector->qtp.min_cluster_pixels = 25;
	d_detector->qtp.max_nmaxima = 10;
	d_detector->qtp.critical_rad = 10.0 * M_PI / 180.0;
	d_detector->qtp.max_line_fit_mse = 10.0;
	d_detector->qtp.min_white_black_diff = threshold;
	d_detector->qtp.deglitch = 0;

}

SnapshotIndexer::~SnapshotIndexer() {
	cancel();
}

void SnapshotIndexer::start() {
	for ( const auto & de : std::filesystem::directory_iterator(d_basedir / d_datadir / "ants" ) ) {
		auto ext = de.path().extension().string();
		std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return std::tolower(c);});
		if ( ext != ".png" ) {
			continue;
		}
		ImageToProcess toProcess = {.Basedir = d_basedir,
		                            .Datadir = d_datadir,
		                            .Path = std::filesystem::relative(de.path(),d_basedir/d_datadir),
		                            .Filter = NULL,
		};
		std::regex filtered("ant_([0-9]+)_frame_([0-9]+).png");
		std::smatch ID;
		std::string filename = de.path().filename().string();
		if(std::regex_search(filename,ID,filtered) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));

			toProcess.Filter = new uint32_t(0);
			IDS >> *(toProcess.Filter);
			FrameS >> toProcess.Frame;
			d_toProcess.push_back(toProcess);
			continue;
		}
		filtered =  std::regex("frame_([0-9]+).png");
		if(std::regex_search(filename,ID,filtered) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(2));
			FrameS >> toProcess.Frame;
			d_toProcess.push_back(toProcess);
			continue;
		}
	}

	d_futureWatcher.setFuture(QtConcurrent::map(d_toProcess,
	                                            [this](ImageToProcess & img) {
		                                            QImage image( (img.Basedir / img.Datadir / img.Path).c_str());
		                                            if ( image.format() != QImage::Format_Grayscale8 ) {
			                                            image = image.convertToFormat(QImage::Format_Grayscale8);
		                                            }
		                                            image_u8_t imgAT = {
		                                                                .width = image.width(),
		                                                                .height = image.height(),
		                                                                .stride = image.bytesPerLine(),
		                                                                .buf =  image.bits(),
		                                            };


		                                            zarray_t * detections = NULL;
		                                            {
			                                            std::lock_guard lock(d_detectorMutex);
			                                            detections = apriltag_detector_detect(d_detector.get(),&imgAT);
		                                            }
		                                            apriltag_detection_t * d;
		                                            img.Results.reserve(zarray_size(detections));
		                                            for ( size_t i = 0; i < zarray_size(detections); ++i ) {
			                                            zarray_get(detections,i,&d);
			                                            if ( img.Filter != NULL && d->id != *(img.Filter) ) {
				                                            continue;
			                                            }
			                                            img.Results.push_back(Snapshot::FromApriltag(d,img.Path, img.Datadir,img.Frame));
		                                            }
	                                            }));
}

void SnapshotIndexer::cancel() {
	d_futureWatcher.cancel();
	d_futureWatcher.waitForFinished();
}


void SnapshotIndexer::onResultReady(int idx) {
	for(const auto & s : d_toProcess[idx].Results ) {
		emit newSnapshot(s);
	}
}
