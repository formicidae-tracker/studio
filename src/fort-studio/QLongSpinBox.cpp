#include "QLongSpinBox.hpp"

#include <QLineEdit>

QLongSpinBox::QLongSpinBox(QWidget * parent)
	: QAbstractSpinBox(parent)
	, d_minimum(0)
	, d_maximum(100)
	, d_value(0) {
	connect(lineEdit(), SIGNAL(textEdited(QString)), this, SLOT(onEditFinished()));
}

QLongSpinBox::~QLongSpinBox() {}


uint64_t QLongSpinBox::minimum() const {
	return d_minimum;
}

uint64_t QLongSpinBox::maximum() const {
	return d_maximum;
}

void QLongSpinBox::setMinimum(uint64_t minimum) {
	d_minimum = minimum;
	if ( d_value < d_minimum ) {
		setValue(d_minimum);
	}
}

void QLongSpinBox::setMaximum(uint64_t maximum) {
	d_maximum = maximum;
	if ( d_value > d_maximum ) {
		setValue(d_maximum);
	}

}

uint64_t QLongSpinBox::value() const {
	return d_value;
}

void QLongSpinBox::setRange(uint64_t min, uint64_t max) {
	if ( min < max ) {
		setMinimum(min);
		setMaximum(max);
	} else {
		setMinimum(max);
		setMaximum(min);
	}
}

void QLongSpinBox::stepBy(int steps) {
	 auto newValue = d_value;
	 if (steps < 0 && newValue + steps > newValue) {
		 newValue = std::numeric_limits<uint64_t>::min();
	 } else if (steps > 0 && newValue + steps < newValue) {
		 newValue = std::numeric_limits<uint64_t>::max();
	 } else {
		 newValue += steps;
	 }

	 lineEdit()->setText(textFromValue(newValue));
	 setValue(newValue);
}

QValidator::State QLongSpinBox::validate(QString &input, int &pos) const {
	bool ok;
	uint64_t value = input.toLongLong(&ok);
	if ( ok == false ) {
		return QValidator::Invalid;
	}

	if (value < d_minimum || value > d_maximum) {
		return QValidator::Invalid;
	}

	return QValidator::Acceptable;
}

uint64_t QLongSpinBox::valueFromText(const QString &text) const {
	return text.toLongLong();
}

QString QLongSpinBox::textFromValue(uint64_t val) const {
	return QString::number(val);
}

QAbstractSpinBox::StepEnabled QLongSpinBox::stepEnabled() const {
	return StepUpEnabled | StepDownEnabled;
}

void QLongSpinBox::setValue(uint64_t value) {
	if ( d_value == value ) {
		return;
	}
	lineEdit()->setText(textFromValue(value));
	d_value = value;
	emit valueChanged(value);
}

void QLongSpinBox::onEditFinished() {
	 QString input = lineEdit()->text();
	 int pos = 0;
	 if (QValidator::Acceptable == validate(input, pos)) {
		 setValue(valueFromText(input));
	 } else {
		 lineEdit()->setText(textFromValue(d_value));
	 }
}
