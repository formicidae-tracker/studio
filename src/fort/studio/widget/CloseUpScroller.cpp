#include "CloseUpScroller.hpp"

#include <fort/studio/bridge/TagCloseUpBridge.hpp>

#include <QBoxLayout>
#include <QSlider>
#include <QLabel>

CloseUpScroller::CloseUpScroller(QWidget * parent)
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
	        &CloseUpScroller::onSliderValueChanged);

	updateWidgets();

}

CloseUpScroller::~CloseUpScroller() {
}

static fmp::TagCloseUp::ConstPtr empty;

const fmp::TagCloseUp::ConstPtr & CloseUpScroller::currentCloseUp() const {
	if ( d_currentCloseUp == d_closeUps.end() ) {
		return empty;
	}
	return *d_currentCloseUp;
}

void CloseUpScroller::setCloseUps(uint32_t objectID,
                                  const QVector<fmp::TagCloseUp::ConstPtr> & closeUps,
                                  QVector<fmp::TagCloseUp::ConstPtr>::const_iterator current) {
	if ( objectID == d_currentID ) {
		return;
	}
	d_currentID = objectID;
	if ( d_currentID == -1 ) {
		d_closeUps.clear();
		d_currentCloseUp = d_closeUps.begin();

	} else {
		d_closeUps = closeUps;
		d_currentCloseUp = d_closeUps.begin() + (current - closeUps.begin());

	}

	updateWidgets();
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpScroller::next() {
	if ( d_currentCloseUp == d_closeUps.end() ) {
		return;
	}

	updateWidgets();
	++d_currentCloseUp;
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpScroller::previous() {
	if ( d_currentCloseUp == d_closeUps.begin() ) {
		return;
	}
	--d_currentCloseUp;
	updateWidgets();
	emit currentCloseUpChanged(currentCloseUp());
}

void CloseUpScroller::onCloseUpsChanged(uint32_t objectID,
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

void CloseUpScroller::updateWidgets() {
	disconnect(d_slider,
	           &QAbstractSlider::valueChanged,
	           this,
	           &CloseUpScroller::onSliderValueChanged);
	d_slider->setRange(0,d_closeUps.size()-1);
	int value = d_currentCloseUp - d_closeUps.begin();
	d_slider->setValue(value);
	d_label->setText(tr("%1/%2").arg(value).arg(d_closeUps.size()));
	connect(d_slider,
	        &QAbstractSlider::valueChanged,
	        this,
	        &CloseUpScroller::onSliderValueChanged);
}

void CloseUpScroller::onSliderValueChanged(int position) {
	if (position >= d_closeUps.size()
	    || d_currentCloseUp == (d_closeUps.begin() + position) ) {
		return;
	}
	d_currentCloseUp = d_closeUps.begin() + position;
	d_label->setText(tr("%1/%2").arg(position).arg(d_closeUps.size()));
	emit currentCloseUpChanged(currentCloseUp());
}


void CloseUpScroller::clear() {
	d_closeUps.clear();
	d_currentCloseUp = d_closeUps.end();
	if ( d_currentID != -1 ) {
		d_currentID = -1;
		emit currentCloseUpChanged(currentCloseUp());
	}
}

AntCloseUpScroller::AntCloseUpScroller(QWidget * parent)
	: CloseUpScroller(parent) {

}

AntCloseUpScroller::~AntCloseUpScroller() {
}

void AntCloseUpScroller::setUp(TagCloseUpBridge * bridge) {
	connect(bridge,
	        &TagCloseUpBridge::cleared,
	        this,
	        &AntCloseUpScroller::clear);

	connect(bridge,
	        &TagCloseUpBridge::closeUpsForAntChanged,
	        this,
	        &AntCloseUpScroller::onCloseUpsChanged);
}


TagCloseUpScroller::TagCloseUpScroller(QWidget * parent)
	: CloseUpScroller(parent) {

}

TagCloseUpScroller::~TagCloseUpScroller() {
}



void TagCloseUpScroller::setUp(TagCloseUpBridge * bridge) {
	connect(bridge,
	        &TagCloseUpBridge::cleared,
	        this,
	        &TagCloseUpScroller::clear);

	connect(bridge,
	        &TagCloseUpBridge::closeUpsForTagChanged,
	        this,
	        &TagCloseUpScroller::onCloseUpsChanged);

}
