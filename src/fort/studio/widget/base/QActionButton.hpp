#pragma once

#include <QPushButton>

class QActionButton : public QPushButton {
	Q_OBJECT
public:
	QActionButton(QWidget * parent = nullptr );
	virtual ~QActionButton();

	void setAction(QAction * action);
private slots:
	void updateButtonState();

private :
	QAction * d_action;
};
