#include "Logger.hpp"

#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QTableView>
#include <QTime>
#include <QHeaderView>


Logger::Logger(QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this)) {
	d_model->setHorizontalHeaderLabels({"Type","Time","Message"});
}

QStandardItemModel * Logger::model() {
	return d_model;
}

void Logger::logMessage(QtMsgType type, const QString & message) {
	auto now = QTime::currentTime();
	QStandardItem * typeItem;
	QFont font;
	QBrush brush(Qt::black);
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

LoggerWidget::LoggerWidget(Logger * logger, QWidget * parent)
	: QWidget(parent) {
	auto layout = new QVBoxLayout();
	setLayout(layout);

	auto tableView = new QTableView(this);
	layout->addWidget(tableView);
	tableView->setModel(logger->model());
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
