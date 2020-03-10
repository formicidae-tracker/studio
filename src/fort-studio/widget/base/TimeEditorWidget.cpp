#include "TimeEditorWidget.hpp"
#include "ui_TimeEditorWidget.h"

#include <fort-studio/Utils.hpp>
#include <fort-studio/Format.hpp>

TimeEditorWidget::TimeEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TimeEditorWidget) {
	d_ui->setupUi(this);
}

TimeEditorWidget::~TimeEditorWidget() {
	delete d_ui;
}

fm::Time::ConstPtr TimeEditorWidget::time() const {
	if ( d_ui->lineEdit->text().isEmpty() ) {
		return fm::Time::ConstPtr();
	}

	try {
		return std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(d_ui->lineEdit->text())));
	} catch ( const std::exception & e) {
		return fm::Time::ConstPtr();
	}
}

void TimeEditorWidget::setTime(const fm::Time::ConstPtr & time) {
	if ( !time ) {
		d_ui->lineEdit->setText("");
		return;
	}
	d_ui->lineEdit->setText(ToQString(*time));
}


void TimeEditorWidget::on_lineEdit_editingFinished() {
	if ( d_ui->lineEdit->text().isEmpty() ) {
		emit timeChanged(fm::Time::ConstPtr());
		return;
	}

	fm::Time::ConstPtr res;
	try {
		res = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(d_ui->lineEdit->text())));
	} catch ( const std::exception & e) {
		return;
	}

	emit timeChanged(res);
}
