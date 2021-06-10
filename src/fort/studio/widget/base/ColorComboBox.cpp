#include "ColorComboBox.hpp"

#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include <QColorDialog>

ColorComboBox::~ColorComboBox() {}

ColorComboBox::ColorComboBox(QWidget * parent)
	: QComboBox(parent)
	, d_color(QColor()) {

	addItem(tr("Custom Color"),QColor());
	insertSeparator(count());
	size_t i = 0;
	for( const auto & fmColor : fm::DefaultPalette() ) {
		auto color = Conversion::colorFromFM(fmColor);
		addItem(Conversion::iconFromFM(fmColor),
		        tr("Color Blind Friendly %1").arg(++i),
		        color);
	}
	connect(this,
	        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
	        this,
	        &ColorComboBox::onActivated);
}

const QColor & ColorComboBox::color() const {
	return d_color;
}

void ColorComboBox::setColor(const QColor & color) {
	if ( color == d_color || color.isValid() == false) {
		return;
	}
	d_color = color;
	emit colorChanged(d_color);
}

void ColorComboBox::onActivated(int index) {
	if ( index < 0 ) {
		return;
	}
	auto color = currentData().value<QColor>();
	if ( color.isValid() == false ) {
		auto newCustom = QColorDialog::getColor(d_color,this,tr("Set a custom color"));
		setColor(newCustom);
		return;
	}
	setColor(color);
}
