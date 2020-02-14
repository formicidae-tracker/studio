#pragma once


#include <QWidget>


class QAbstractItemModel;
class QStandardItemModel;

class LoggerWidget;

class Logger : public QObject {
	Q_OBJECT
public:
	Logger(QObject * parent);

	QStandardItemModel * model();

public slots:
	void logMessage(QtMsgType type, const QString & message);

private:
	QStandardItemModel * d_model;
};

class LoggerWidget : public QWidget {
	Q_OBJECT
public:
	LoggerWidget(Logger * logger, QWidget * parent);
};
