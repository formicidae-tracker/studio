#pragma once

#include <QObject>

class Bridge : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool modified
	           READ isModified
	           NOTIFY modified)
	Q_PROPERTY(bool active
	           READ isActive
	           NOTIFY activated)
public:
	Bridge(QObject * parent);

	bool isModified() const;

	virtual bool isActive() const = 0;

signals:
	void modified(bool);
	void activated(bool);

public slots:
	void setModified(bool);

private:
	bool d_modified;
};
