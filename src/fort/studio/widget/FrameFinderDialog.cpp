#include "FrameFinderDialog.hpp"
#include "ui_FrameFinderDialog.h"

#include <fort/studio/bridge/UniverseBridge.hpp>
#include <fort/studio/Format.hpp>

#include <QDebug>

FrameFinderDialog::FrameFinderDialog(UniverseBridge * universe,
                                     QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::FrameFinderDialog) {
	d_ui->setupUi(this);
	setWindowModality(Qt::ApplicationModal);

	setWindowTitle(tr("Find a Frame"));

	const auto & tdds = universe->trackingDataDirectories();

	for ( const auto & [uri,tdd] : tdds ) {
		d_ui->comboBox->addItem(uri.c_str(),QVariant::fromValue(tdd));
	}
	d_ui->comboBox->setCurrentIndex(-1);
	d_ui->spinBox->setEnabled(false);
}

FrameFinderDialog::~FrameFinderDialog() {
	delete d_ui;
}


fmp::FrameReference::ConstPtr FrameFinderDialog::Get(UniverseBridge * universe,
                                                     QWidget * parent) {
	FrameFinderDialog dialog(universe,parent);
	QEventLoop loop;
	connect(&dialog, &QDialog::finished,&loop, &QEventLoop::quit);
	dialog.open();
	loop.exec();
	if ( dialog.result() == QDialog::Rejected || dialog.d_ui->comboBox->currentIndex() < 0 ) {
		return fmp::FrameReference::ConstPtr();
	}
	auto tdd = dialog.d_ui->comboBox->currentData().value<fmp::TrackingDataDirectory::ConstPtr>();
	if ( !tdd ) {
		return fmp::FrameReference::ConstPtr();
	}

	try {
		uint64_t frameID = dialog.d_ui->spinBox->value();
		return std::make_shared<fmp::FrameReference>(tdd->FrameReferenceAt(frameID));
	} catch ( const std::exception & e ) {
		qCritical() << "Could not retrieve frame reference "
		            << ToQString(tdd->URI())
		            << "/frames/" << dialog.d_ui->spinBox->value()
		            << ": " << e.what();
	};

	return fmp::FrameReference::ConstPtr();
}


void FrameFinderDialog::on_comboBox_currentIndexChanged(int index) {
	if ( index < 0 ) {
		d_ui->spinBox->setEnabled(false);
		return;
	}
	auto tdd = d_ui->comboBox->currentData().value<fmp::TrackingDataDirectory::ConstPtr>();
	if (!tdd) {
		d_ui->spinBox->setEnabled(false);
		return;
	}
	d_ui->spinBox->setMinimum(tdd->StartFrame());
	d_ui->spinBox->setMaximum(tdd->EndFrame());
	d_ui->spinBox->setToolTip(tr("Set FrameID in [%1;%2]").arg(tdd->StartFrame()).arg(tdd->EndFrame()));
	d_ui->spinBox->setEnabled(true);

}
