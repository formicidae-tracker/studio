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

	void saveLogAs();
private:
	QStandardItemModel * d_model;
};

namespace Ui {
class LoggerWidget;
}

class QSortFilterProxyModel;

class LoggerWidget : public QWidget {
	Q_OBJECT
public:
	explicit LoggerWidget(Logger * logger, QWidget * parent = 0);
	~LoggerWidget();

private slots:
	void onDisplayLogLevelChanged(int);

private:
	Ui::LoggerWidget      * d_ui;
	QSortFilterProxyModel * d_filteredModel;
};
