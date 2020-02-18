#include "ColorComboBox.hpp"

#include <myrmidon/priv/Color.hpp>

#include <QColorDialog>

ColorComboBox::ColorComboBox(QWidget * parent)
	: QComboBox(parent)
	, d_color(QColor()) {

	addItem(tr("Custom Color"),QColor());
	insertSeparator(count());
	const auto & palette = fmp::Palette::Default();
	for( size_t i = 0 ; i < palette.Size(); ++i ) {
		auto color = fromMyrmidon(palette.At(i));
		addItem(iconFromColor(color),
		        tr("Color Blind Friendly %1").arg(i+1),
		        color);
	}
	connect(this,
	        static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
	        this,
	        &ColorComboBox::onActivated);
}

QColor ColorComboBox::fromMyrmidon(const fmp::Color & color) {
	return QColor(std::get<0>(color),
	              std::get<1>(color),
	              std::get<2>(color));
}

QIcon ColorComboBox::iconFromColor(const QColor & color) {
	QPixmap icon(15,15);
	icon.fill(color);
	return icon;
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
