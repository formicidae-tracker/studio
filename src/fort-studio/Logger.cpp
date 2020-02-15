#include "Logger.hpp"
#include "ui_LoggerWidget.h"

#include <QStandardItemModel>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>

Logger::Logger(QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this)) {
	d_model->setHorizontalHeaderLabels({"Type","Time","Message"});
}

QStandardItemModel * Logger::model() {
	return d_model;
}

void Logger::logMessage(QtMsgType type, const QString & message) {
	auto now = QDateTime::currentDateTime();
	QStandardItem * typeItem;
	QFont font;
	QBrush brush(QApplication::palette((QWidget*)NULL).color(QPalette::Foreground));
	switch(type) {
	case QtDebugMsg:
		typeItem = new QStandardItem("D");
		font.setItalic(true);
		brush = Qt::blue;
		break;
	case QtInfoMsg:
		typeItem = new QStandardItem("I");
		break;
	case QtWarningMsg:
		typeItem = new QStandardItem("W");
		font.setBold(true);
		brush = QColor(255,174,66);
		break;
	case QtCriticalMsg:
		typeItem = new QStandardItem("E");
		font.setBold(true);
		brush = Qt::red;
		break;
	case QtFatalMsg:
		typeItem = new QStandardItem("F");
		font.setBold(true);
		font.setBold(true);
		brush = Qt::red;
		break;
	}
	auto timeItem = new QStandardItem(now.toString());
	auto messageItem = new QStandardItem(message);
	QList<QStandardItem *> res = {typeItem,timeItem,messageItem};
	for ( const auto & i : res ) {
		i->setEditable(false);
		i->setFont(font);
		i->setForeground(brush);
	}

	d_model->appendRow(res);
}

void Logger::saveLogAs() {
	auto filepath = QFileDialog::getSaveFileName(NULL,tr("Save Log"),"",tr("Text files (*.txt)"));
	if ( filepath.isEmpty() == true ) {
		qDebug() << "Not saving log as user did not set a path";
		return;
	}
	QFile logFile(filepath);
	qDebug() << "Opening '" << filepath << "'";
	if ( logFile.open(QIODevice::WriteOnly | QIODevice::Text) == false ) {
		qCritical() << "Could not open '" << filepath << "': " << logFile.errorString();
		return;
	}
	QTextStream out(&logFile);
	for ( size_t i = 0 ; i < d_model->rowCount() ; ++i ) {
		out << d_model->item(i,0)->text()
		    << ":" << d_model->item(i,1)->text()
		    << ":" << d_model->item(i,2)->text()
		    << ";\n";
	}
	qInfo() << "Dumpped log to '" << filepath << "'";
}


LoggerWidget::LoggerWidget(Logger * logger, QWidget * parent)
	: QWidget(parent)
	, d_ui ( new Ui::LoggerWidget)
	, d_filteredModel( new QSortFilterProxyModel(this) ) {

	d_filteredModel->setSourceModel(logger->model());

	d_ui->setupUi(this);


	d_ui->tableView->setModel(d_filteredModel);

	d_ui->comboBox->insertItem(0,"Debug","[DIWE]");
	d_ui->comboBox->insertItem(1,"Info","[IWE]");
	d_ui->comboBox->insertItem(2,"Warning","[WE]");
	d_ui->comboBox->insertItem(3,"Error","[E]");

	connect(d_ui->pushButton,
	        &QPushButton::clicked,
	        logger,
	        &Logger::saveLogAs);

	connect(d_ui->comboBox,
	        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
	        this,
	        &LoggerWidget::onDisplayLogLevelChanged);

	d_ui->comboBox->setCurrentIndex(1);
}

LoggerWidget::~LoggerWidget() {
	delete d_ui;
}

void LoggerWidget::onDisplayLogLevelChanged(int index) {
	d_filteredModel->setFilterKeyColumn(0);
	d_filteredModel->setFilterRegExp(d_ui->comboBox->currentData().toString());
}
