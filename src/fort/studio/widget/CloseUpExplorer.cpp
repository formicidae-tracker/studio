#include "CloseUpExplorer.hpp"

#include <fort/studio/bridge/TagCloseUpBridge.hpp>

#include <QBoxLayout>
#include <QSlider>
#include <QLabel>

CloseUpExplorer::CloseUpExplorer(QWidget * parent)
	: QWidget(parent)
	, d_currentID(-1) {
	d_currentCloseUp = d_closeUps.end();

	auto layout = new QHBoxLayout(this);
	d_label = new QLabel(this);
	layout->addWidget(d_label);
	d_slider = new QSlider(Qt::Horizontal,this);
	layout->addWidget(d_slider);

	connect(d_slider,
	        &QAbstractSlider::valueChanged,
	        this,
	        &CloseUpExplorer::onSliderValueChanged);

	updateWidgets();

}

CloseUpExplorer::~CloseUpExplorer() {
}

static fmp::TagCloseUp::ConstPtr empty;

const fmp::TagCloseUp::ConstPtr & CloseUpExplorer::currentCloseUp() const {
	if ( d_currentCloseUp == d_closeUps.end() ) {
		return empty;
	}
	return *d_currentCloseUp;
}

void CloseUpExplorer::setCloseUps(uint32_t objectID,
                                  const QVector<fmp::TagCloseUp::ConstPtr> & closeUps) {
	if ( objectID == d_currentID ) {
		return;
	}
	d_currentID = objectID;
	if ( d_currentID == -1 ) {
		d_closeUps.clear();
	} else {
		d_closeUps = closeUps;
	}

	d_currentCloseUp = d_closeUps.begin();
	updateWidgets();
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpExplorer::next() {
	if ( d_currentCloseUp == d_closeUps.end() ) {
		return;
	}

	updateWidgets();
	++d_currentCloseUp;
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpExplorer::previous() {
	if ( d_currentCloseUp == d_closeUps.begin() ) {
		return;
	}
	--d_currentCloseUp;
	updateWidgets();
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpExplorer::onCloseUpsChanged(uint32_t objectID,
                                        const QVector<fmp::TagCloseUp::ConstPtr> & closeUps) {
	if ( objectID != d_currentID ) {
		return;
	}

	bool changed = false;
	// here is the super tricky part: we must found back the current closeUp in the new list
	while( d_currentCloseUp != d_closeUps.end()
	       && std::count(closeUps.begin(),closeUps.end(),*d_currentCloseUp) == 0 ) {
		changed = true;
		++d_currentCloseUp;
	}
	if ( d_currentCloseUp == d_closeUps.end()
	     && closeUps.empty() == false ) {
		changed = true;
		d_currentCloseUp = closeUps.begin();
	}
	if ( d_currentCloseUp != d_closeUps.end() ) {
		d_currentCloseUp = std::find(closeUps.begin(),closeUps.end(),*d_currentCloseUp);
	}

	d_closeUps = closeUps;

	updateWidgets();

	if (changed == true ) {
		emit currentCloseUpChanged(currentCloseUp());
	}
}

void CloseUpExplorer::updateWidgets() {
	disconnect(d_slider,
	           &QAbstractSlider::valueChanged,
	           this,
	           &CloseUpExplorer::onSliderValueChanged);
	d_slider->setRange(0,d_closeUps.size()-1);
	int value = d_currentCloseUp - d_closeUps.begin();
	d_slider->setValue(value);
	d_label->setText(tr("%1/%2").arg(value).arg(d_closeUps.size()));
	connect(d_slider,
	        &QAbstractSlider::valueChanged,
	        this,
	        &CloseUpExplorer::onSliderValueChanged);
}

void CloseUpExplorer::onSliderValueChanged(int position) {
	if (position >= d_closeUps.size()
	    || d_currentCloseUp == (d_closeUps.begin() + position) ) {
		return;
	}
	d_currentCloseUp = d_closeUps.begin() + position;
	d_label->setText(tr("%1/%2").arg(position).arg(d_closeUps.size()));
	emit currentCloseUpChanged(currentCloseUp());
}


AntCloseUpExplorer::AntCloseUpExplorer(QWidget * parent)
	: CloseUpExplorer(parent) {

}

AntCloseUpExplorer::~AntCloseUpExplorer() {
}

void AntCloseUpExplorer::setUp(TagCloseUpBridge * bridge) {
	connect(bridge,
	        &TagCloseUpBridge::closeUpsForAntChanged,
	        this,
	        &AntCloseUpExplorer::onCloseUpsChanged);
}


TagCloseUpExplorer::TagCloseUpExplorer(QWidget * parent)
	: CloseUpExplorer(parent) {

}

TagCloseUpExplorer::~TagCloseUpExplorer() {
}



void TagCloseUpExplorer::setUp(TagCloseUpBridge * bridge) {
	connect(bridge,
	        &TagCloseUpBridge::closeUpsForTagChanged,
	        this,
	        &TagCloseUpExplorer::onCloseUpsChanged);

}
