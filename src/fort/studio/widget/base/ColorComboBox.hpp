#pragma once

#include <QComboBox>
#include <QColor>


class ColorComboBox : public QComboBox {
	Q_OBJECT
	Q_PROPERTY(QColor color
	           READ color WRITE setColor
	           NOTIFY colorChanged)
public:
	ColorComboBox(QWidget * parent);
	virtual ~ColorComboBox();

	const QColor & color() const;

public slots:
	void setColor(const QColor & color);

private slots:
	void onActivated(int index);

signals:
	void colorChanged(const QColor & color);

private:
	const static int CUSTOM_COLOR_INDEX = 0;

	QColor d_color;
};
