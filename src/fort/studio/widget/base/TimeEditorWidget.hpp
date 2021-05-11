#pragma once

#include <QWidget>

#include <fort/studio/MyrmidonTypes/Time.hpp>


namespace Ui {
class TimeEditorWidget;
}

class QAction;
class UniverseBridge;
class QCalendarWidget;
class QTimeEdit;

class TimeEditorWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(fort::Time time
	           READ time
	           WRITE setTime
	           NOTIFY timeChanged)
public:
	explicit TimeEditorWidget(QWidget *parent = 0);
	virtual ~TimeEditorWidget();

	fort::Time time() const;

	void setup(UniverseBridge * universe);

public slots:
	void setTime(const fort::Time & time);


	void increment();
	void decrement();
private slots:
	void on_lineEdit_editingFinished();
	void on_frameButton_clicked();

	void onPopup();

signals:
	void timeChanged(const fort::Time & time);

private:
	void incrementBy(fort::Duration duration);

	Ui::TimeEditorWidget * d_ui;
	QAction              * d_increment;
	QAction              * d_decrement;
	QAction              * d_popup;
	QAction              * d_warning;
	UniverseBridge       * d_universe;
	QWidget              * d_popupWidget;
	QCalendarWidget      * d_calendar;
	QTimeEdit            * d_timeEdit;
};
