#pragma once

#include <QSpinBox>

class QLongSpinBox : public QAbstractSpinBox {
	Q_OBJECT

	Q_PROPERTY(uint64_t minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(uint64_t maximum READ maximum WRITE setMaximum)

    Q_PROPERTY(uint64_t value READ value WRITE setValue NOTIFY valueChanged USER true)
public:
	QLongSpinBox(QWidget * parent = NULL);
	virtual ~QLongSpinBox();

	uint64_t minimum() const;
	uint64_t maximum() const;

	void setMinimum(uint64_t minimum);
	void setMaximum(uint64_t maximum);

	uint64_t value() const;

	void setRange(uint64_t min, uint64_t max);

	void stepBy(int steps) override;

 protected:

	QValidator::State validate(QString &input, int &pos) const override;

	virtual uint64_t valueFromText(const QString &text) const;

	virtual QString textFromValue(uint64_t val) const;

	virtual QAbstractSpinBox::StepEnabled stepEnabled() const;

 public slots:

	void setValue(uint64_t value);

	void onEditFinished();

 signals:
	void valueChanged(uint64_t v);

private:
	Q_DISABLE_COPY(QLongSpinBox)

	uint64_t d_minimum,d_maximum,d_value;
};
