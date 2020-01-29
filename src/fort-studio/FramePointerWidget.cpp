#include "FramePointerWidget.hpp"
#include "ui_FramePointerWidget.h"

#include <QDebug>

#include <myrmidon/priv/RawFrame.hpp>

using namespace fort::myrmidon::priv;

FramePointerWidget::FramePointerWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::FramePointerWidget)
	, d_inhibit(false) {
	d_ui->setupUi(this);
}

FramePointerWidget::~FramePointerWidget() {
	delete d_ui;
}


void FramePointerWidget::setTitle(const QString & title) {
	d_ui->groupBox->setTitle(title);
}

void FramePointerWidget::onNewController(ExperimentController * controller ) {
	d_controller = controller;

	if ( d_controller != NULL ) {
		disconnect(d_controller,
		           SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & )),
		           this,
		           SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & )));
	}


	if ( d_controller == NULL ) {
		d_ui->comboBox->clear();
		setEnabled(false);
		return;
	}
	setEnabled(true);

	connect(d_controller,
	        SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & )),
	        this,
	        SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & )));


	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
}


void FramePointerWidget::onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & tdds) {
	d_inhibit = true;
	fs::path selected;
	if ( d_ui->comboBox->currentIndex() >= 0 ) {
		selected = d_ui->comboBox->currentData().toString().toUtf8().constData();
	}

	d_ui->comboBox->clear();
	std::vector<const TrackingDataDirectory*> sorted;
	sorted.reserve(tdds.size());
	for ( const auto & el : tdds ) {
		sorted.push_back(&el.second);
	}

	priv::TimeValid::SortAndCheckOverlap(sorted.begin(),sorted.end());
	size_t i = 0;
	bool set = false;
	for ( const auto & tdd : sorted ) {
		d_ui->comboBox->insertItem(i,tdd->Path().generic_string().c_str(),tdd->Path().generic_string().c_str());
		if ( tdd->Path() == selected ) {
			d_ui->comboBox->setCurrentIndex(i);
			set = true;
		}
		i++;
	}
	if ( set == false ) {
		d_ui->comboBox->setCurrentIndex(-1);
	}
	d_inhibit = false;
}


void FramePointerWidget::on_comboBox_currentIndexChanged(int i) {
	if (i < 0) {
		d_inhibit = true;
		d_ui->spinBox->setEnabled(false);
		d_ui->spinBox->setMinimum(0);
		d_ui->spinBox->setMaximum(0);
		d_ui->spinBox->setToolTip(tr("Frame ∈ [%1;%2]").arg(0).arg(0));
		d_inhibit = false;
		return;
	}
	d_ui->spinBox->setEnabled(true);

	fs::path path = d_ui->comboBox->currentData().toString().toUtf8().constData();
	auto & tdd = d_controller->experiment().TrackingDataDirectories().find(path.generic_string())->second;

	d_ui->spinBox->setMinimum(tdd.StartFrame() == 0 ? 0 : tdd.StartFrame()-1);
	d_ui->spinBox->setMaximum(tdd.EndFrame() + 1);
	d_ui->spinBox->setToolTip(tr("Frame ∈ [%1;%2]").arg(tdd.StartFrame()).arg(tdd.EndFrame()));
	d_inhibit = false;
}


void FramePointerWidget::on_groupBox_toggled(bool enabled) {
	if ( enabled == false ) {
		emit framePointerUpdated(RawFrame::ConstPtr());
		d_ui->comboBox->setCurrentIndex(-1);
	}
}


void FramePointerWidget::on_spinBox_valueChanged(uint64_t value) {
	if ( d_inhibit == true ) {
		return;
	}
	auto & tdds = d_controller->experiment().TrackingDataDirectories();
	auto fi = tdds.find(d_ui->comboBox->currentData().toString().toUtf8().constData());
	if ( fi == tdds.end() ){
		return;
	}
	try {
		emit framePointerUpdated(*fi->second.FrameAt(value));
	} catch ( const std::exception & ) {

	}
}
