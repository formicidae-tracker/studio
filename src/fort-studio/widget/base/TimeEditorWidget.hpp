#pragma once

#include <QWidget>

#include <fort-studio/MyrmidonTypes.hpp>


namespace Ui {
class TimeEditorWidget;
}

class TimeEditorWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(fm::Time::ConstPtr time
	           READ time
	           WRITE setTime
	           NOTIFY timeChanged)
public:
	explicit TimeEditorWidget(QWidget *parent = 0);
	~TimeEditorWidget();

	fm::Time::ConstPtr time() const;

public slots:
	void setTime(const fm::Time::ConstPtr & time);

private slots:
	void on_lineEdit_editingFinished();

signals:
	void timeChanged(const fm::Time::ConstPtr & time);

private:
	Ui::TimeEditorWidget * d_ui;
};
