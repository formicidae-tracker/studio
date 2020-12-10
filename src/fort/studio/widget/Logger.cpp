#include "Logger.hpp"
#include "ui_LoggerWidget.h"

#include <QDebug>
#include <QDateTime>
#include <QFontDatabase>

#include <glog/logging.h>

Logger::~Logger() {}

Logger::Logger(QObject * parent)
	: QObject(parent)
	, d_warningCounts(0)
	, d_errorCounts(0) {
	d_loggerData =
		{
#ifndef NDEBUG
		 {QtDebugMsg,
		  {
		   [](const QString & msg, const QDateTime & time) -> QString{
			   return "I " + time.toString() + " : " + msg;
		   },
		   [](const QString & msg) -> QString {
			   return "<span style=\"color:#0000ff;\">" + msg + "</span>";
		   },
		   [](const QString & msg) {
			   LOG(INFO) << msg.toUtf8().constData();
		   },
		   {&d_infos}
		  }
		 },
#endif
		 {QtInfoMsg,
		  {
		   [](const QString & msg, const QDateTime & time) -> QString{
			   return "I " + time.toString() + " : " + msg;
		   },
		   [](const QString & msg) -> QString {
			   return msg;
		   },
		   [](const QString & msg) {
			   LOG(INFO) << msg.toUtf8().constData();
		   },
		   {&d_infos}
		  }
		 },
		 {QtWarningMsg,
		  {
		   [](const QString & msg, const QDateTime & time) -> QString{
			   return "W " + time.toString() + " : " + msg;
		   },
		   [](const QString & msg) -> QString {
			   return "<span style=\"color:#b8860b;\">" + msg + "</span>";
		   },
		   [this](const QString & msg) {
			   ++d_warningCounts;
			   emit warningCountChanged(d_warningCounts);
			   LOG(WARNING) << msg.toUtf8().constData();
		   },
		   {&d_infos,&d_warnings}
		  }
		 },
		 {QtCriticalMsg,
		  {
		   [](const QString & msg, const QDateTime & time) -> QString{
			   return "E " + time.toString() + " : " + msg;
		   },
		   [](const QString & msg) -> QString {
			   return "<span style=\"color:#ff0000;\">" + msg + "</span>";
		   },
		   [this](const QString & msg) {
			   ++d_errorCounts;
			   emit errorCountChanged(d_errorCounts);
			   LOG(ERROR) << msg.toUtf8().constData();
		   },
		   {&d_infos,&d_warnings,&d_errors}
		  }
		 },
		 {QtFatalMsg,
		  {
		   [](const QString & msg, const QDateTime & time) -> QString{
			   return "F " + time.toString() + " : " + msg;
		   },
		   [](const QString & msg) -> QString {
			   return "<span style=\"color:#ff0000;\">" + msg + "</span>";
		   },
		   [this](const QString & msg) {
			   LOG(FATAL) << msg.toUtf8().constData();
		   },
		   {}
		  }
		 },
		};
}


int Logger::warningCount() const {
	return d_warningCounts;
}

int Logger::errorCount() const {
	return d_errorCounts;
}

#include <iostream>

void Logger::logMessage(QtMsgType type, const QString & message) {
	auto fi = d_loggerData.find(type);
	if ( fi == d_loggerData.end() ) {
		return;
	}
	auto now = QDateTime::currentDateTime();

	auto logMessage = fi->second.FormatLog(message,now);
	auto stylized = fi->second.StylizeText(logMessage);
	for ( const auto & d : fi->second.Destinations ) {
		*d += stylized + "<br>";
	}
	fi->second.Process(logMessage);

	emit newMessage(type,message);
}


const QString & Logger::infos() const {
	return d_infos;
}

const QString & Logger::warnings() const {
	return d_warnings;
}

const QString & Logger::errors() const {
	return d_errors;
}

LoggerWidget::LoggerWidget(Logger * logger, QWidget * parent)
	: QWidget(parent)
	, d_ui ( new Ui::LoggerWidget)
	, d_logger(logger) {



	d_ui->setupUi(this);


	d_ui->comboBox->insertItem(0,"Info");
	d_ui->comboBox->insertItem(1,"Warning");
	d_ui->comboBox->insertItem(2,"Error");

	setWindowTitle(tr("FORT Studio Log (/tmp/fort-studio.INFO)"));

	connect(d_ui->comboBox,
	        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
	        this,
	        &LoggerWidget::onNewMessage);

	d_ui->comboBox->setCurrentIndex(0);

	connect(d_logger,
	        &Logger::newMessage,
	        this,
	        &LoggerWidget::onNewMessage,
	        Qt::QueuedConnection);
	onNewMessage();

	const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	d_ui->textEdit->setFont(fixedFont);
}



LoggerWidget::~LoggerWidget() {
	delete d_ui;
}

void LoggerWidget::onNewMessage() {
	switch(d_ui->comboBox->currentIndex() ) {
	case 0:
		setRichText(d_logger->infos());
		break;
	case 1:
		setRichText(d_logger->warnings());
		break;
	case 3:
		setRichText(d_logger->errors());
		break;
	default:
		break;
	}
}

void LoggerWidget::setRichText(const QString & msg) {
	d_ui->textEdit->setHtml(msg + "<span id=\"endOfLogs\"></span>");
	d_ui->textEdit->scrollToAnchor("endOfLogs");
}

LogStatusWidget::LogStatusWidget(Logger * logger,QWidget * parent)
	: QWidget(parent) {
	d_errorIcon = new QLabel(this);
	d_errorLabel = new QLabel(this);
	d_warningIcon = new QLabel(this);
	d_warningLabel = new QLabel(this);
	d_message = new QLabel(this);

	auto layout = new QHBoxLayout(this);
	layout->addWidget(d_errorIcon);
	layout->addWidget(d_errorLabel);
	layout->addWidget(d_warningIcon);
	layout->addWidget(d_warningLabel);
	layout->addWidget(d_message);
	setLayout(layout);

	setToolTip(tr("Double-click to show log"));
	setStatusTip(tr("Double-click to show log"));
	connect(logger,
	        &Logger::warningCountChanged,
	        this,
	        [this] ( int warnings) {
		        d_warningLabel->setText(tr(": %1").arg(warnings));
		        d_warningIcon->setEnabled(warnings > 0);
	        },
	        Qt::QueuedConnection);

	connect(logger,
	        &Logger::errorCountChanged,
	        this,
	        [this] ( int errors) {
		        d_errorLabel->setText(tr(": %1").arg(errors));
		        d_errorIcon->setEnabled(errors > 0);
	        },
	        Qt::QueuedConnection);

	connect(logger,
	        &Logger::newMessage,
	        this,
	        &LogStatusWidget::onNewMessage,
	        Qt::QueuedConnection);

	d_errorIcon->setPixmap(QIcon::fromTheme("dialog-error-symbolic").pixmap(15,15));
	d_warningIcon->setPixmap(QIcon::fromTheme("dialog-warning-symbolic").pixmap(15,15));

	const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

	setFont(fixedFont);
	d_message->setFont(fixedFont);

	d_warningLabel->setText(": 0");
	d_errorLabel->setText(": 0");
	d_warningIcon->setEnabled(false);
	d_errorIcon->setEnabled(false);
	onNewMessage(QtWarningMsg,"");

}

LogStatusWidget::~LogStatusWidget() {
}


void LogStatusWidget::onNewMessage(int type,const QString & message) {
	const static int SIZE = 60;
	auto actualMessage = message;
	if ( actualMessage.size() < SIZE ) {
		actualMessage.resize(SIZE,' ');
	} else if ( actualMessage.size() > SIZE ) {
		actualMessage.resize(SIZE-3);
		actualMessage += "...";
	}

	switch(type) {
	case QtWarningMsg:
		d_message->setText(actualMessage);
		d_message->setStyleSheet("QLabel {}");
		break;
	case QtCriticalMsg:
		d_message->setText(actualMessage);
		d_message->setStyleSheet("QLabel {color : red;}");
		QApplication::beep();
		break;
	default:
		break;
	}
}

void LogStatusWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	emit showLog();
}
