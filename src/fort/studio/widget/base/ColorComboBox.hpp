#pragma once

#include <QComboBox>
#include <QColor>

#include <fort/myrmidon/priv/Color.hpp>


namespace fmp = fort::myrmidon::priv;

class ColorComboBox : public QComboBox {
	Q_OBJECT
	Q_PROPERTY(QColor color
	           READ color WRITE setColor
	           NOTIFY colorChanged)
public:
	static QIcon  iconFromColor(const QColor & color);

	ColorComboBox(QWidget * parent);

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
