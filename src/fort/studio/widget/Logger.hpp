#pragma once


#include <QWidget>
#include <QDateTime>

#include <functional>

class QLabel;
class LoggerWidget;

class Logger : public QObject {
	Q_OBJECT
	Q_PROPERTY(int warningCount
	           READ warningCount
	           NOTIFY warningCountChanged);
	Q_PROPERTY(int errorCount
	           READ errorCount
	           NOTIFY errorCountChanged);
public:
	explicit Logger(QObject * parent);
	virtual ~Logger();

	const QString & infos() const;
	const QString & warnings() const;
	const QString & errors() const;

	int warningCount() const;
	int errorCount() const;

signals:
	void newMessage(int type, const QString & message);

	void warningCountChanged(int);
	void errorCountChanged(int);
public slots:
	void logMessage(QtMsgType type, const QString & message);

private:
	typedef std::function<QString(const QString & msg, const QDateTime & time)> LogFormatter;
	typedef std::function<QString(const QString & msg)> RichTextFormatter;
	typedef std::function<void(const QString & msg)> Processor;
	struct LoggerData {
		LogFormatter          FormatLog;
		RichTextFormatter     StylizeText;
		Processor             Process;
		std::vector<QString*> Destinations;
	};

	std::map<QtMsgType,LoggerData> d_loggerData;
	QString d_infos,d_warnings,d_errors;
	int d_warningCounts,d_errorCounts;
};

namespace Ui {
class LoggerWidget;
}



class LoggerWidget : public QWidget {
	Q_OBJECT
public:
	explicit LoggerWidget(Logger * logger, QWidget * parent = 0);
	virtual ~LoggerWidget();

private slots:
	void onNewMessage();


private:
	void setRichText(const QString & txt);
	Ui::LoggerWidget      * d_ui;
	Logger                * d_logger;

};


class LogStatusWidget : public QWidget {
	Q_OBJECT
public:
	explicit LogStatusWidget(Logger * logger,QWidget * parent = nullptr);
	virtual ~LogStatusWidget();

protected:
	void mouseDoubleClickEvent(QMouseEvent * event) override;

protected slots:

	void onNewMessage(int type,const QString & message);


signals:
	void showLog();

private:
	QLabel *d_warningLabel,
		*d_warningIcon,
		*d_errorLabel,
		*d_errorIcon,
		*d_message;
};
