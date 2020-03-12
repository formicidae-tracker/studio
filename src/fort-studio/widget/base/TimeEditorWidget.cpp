#include "TimeEditorWidget.hpp"
#include "ui_TimeEditorWidget.h"

#include <fort-studio/Utils.hpp>
#include <fort-studio/Format.hpp>

#include <QIcon>

#include <fort-studio/widget/FrameFinderDialog.hpp>


TimeEditorWidget::TimeEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TimeEditorWidget)
	, d_universe(nullptr) {
	d_ui->setupUi(this);


	d_warning = d_ui->lineEdit->addAction(QIcon::fromTheme("dialog-warning"),QLineEdit::TrailingPosition);
	d_warning->setVisible(false);
	d_popup = d_ui->lineEdit->addAction(QIcon::fromTheme("edit-find"),QLineEdit::TrailingPosition);
	d_decrement = d_ui->lineEdit->addAction(QIcon::fromTheme("list-remove"),QLineEdit::TrailingPosition);
	d_increment = d_ui->lineEdit->addAction(QIcon::fromTheme("list-add"),QLineEdit::TrailingPosition);

	d_popup->setEnabled(false);
	d_decrement->setEnabled(false);
	d_increment->setEnabled(false);

	connect(d_increment,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::increment);

	connect(d_decrement,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::decrement);

	connect(d_popup,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::onPopup);

	d_ui->frameButton->setEnabled(false);
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
	d_warning->setVisible(false);
	if ( !time ) {
		d_ui->lineEdit->setText("");
		d_decrement->setEnabled(false);
		d_increment->setEnabled(false);
		return;
	}
	d_decrement->setEnabled(true);
	d_increment->setEnabled(true);

	d_ui->lineEdit->setText(ToQString(*time));
}


void TimeEditorWidget::on_lineEdit_editingFinished() {
	if ( d_ui->lineEdit->text().isEmpty() == true ) {
		setTime(fm::Time::ConstPtr());
		emit timeChanged(fm::Time::ConstPtr());
		return;
	}

	fm::Time::ConstPtr res;
	try {
		res = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(d_ui->lineEdit->text())));
	} catch ( const std::exception & e) {
		d_warning->setVisible(true);
		return;
	}
	d_warning->setVisible(false);

	emit timeChanged(res);
}

void TimeEditorWidget::increment() {
	incrementBy(1 * fm::Duration::Second);
}

void TimeEditorWidget::decrement() {
	incrementBy(-1 * fm::Duration::Second);
}

void TimeEditorWidget::incrementBy(fm::Duration duration) {
	if ( d_ui->lineEdit->text().isEmpty() == true ) {
		return;
	}
	fm::Time::ConstPtr res;
	try {
		res = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(d_ui->lineEdit->text())));
	} catch ( const std::exception & e ) {
		return;
	}
	res = std::make_shared<fm::Time>(res->Add(duration));
	setTime(res);
	emit timeChanged(res);
}


void TimeEditorWidget::setup(UniverseBridge * universe) {
	d_universe = universe;
	d_popup->setEnabled(true);
	d_ui->frameButton->setEnabled(true);
}

void TimeEditorWidget::onPopup() {

}


void TimeEditorWidget::on_frameButton_clicked() {
	if ( d_universe == nullptr ) {
		return;
	}
	auto ref = FrameFinderDialog::Get(d_universe,this);
	if ( !ref ) {
		return;
	}
	auto time = std::make_shared<fm::Time>(ref->Time());
	setTime(time);
	emit timeChanged(time);
}
