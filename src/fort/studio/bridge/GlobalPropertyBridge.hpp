#pragma once

#include "Bridge.hpp"

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/MyrmidonTypes/Types.hpp>

namespace fmp = fort::myrmidon::priv;

class GlobalPropertyBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(QString name
	           READ name WRITE setName
	           NOTIFY nameChanged)
	Q_PROPERTY(QString author
	           READ author WRITE setAuthor
	           NOTIFY authorChanged)
	Q_PROPERTY(QString comment
	           READ comment WRITE setComment
	           NOTIFY commentChanged)
	Q_PROPERTY(fort::tags::Family tagFamily
	           READ tagFamily NOTIFY tagFamilyChanged)
	Q_PROPERTY(double tagSize
	           READ tagSize WRITE setTagSize
	           NOTIFY tagSizeChanged)

public:
	GlobalPropertyBridge(QObject * parent);
	virtual ~GlobalPropertyBridge();

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

public:
	QString name() const;
	QString author() const;
	QString comment() const;
	fort::tags::Family tagFamily() const;
	double tagSize() const;

signals:
	void nameChanged(QString name);
	void authorChanged(QString author);
	void commentChanged(QString comment);
	void tagFamilyChanged(fort::tags::Family f);
	void tagSizeChanged(double value);

public slots:
	void onTDDModified();


	void setName(const QString & name);
	void setAuthor(const QString & author);
	void setComment(const QString & comment,bool noSignal = false);
	void setTagSize(double tagSize);

private:
	fmp::Experiment::Ptr d_experiment;
	fort::tags::Family   d_cached;
};
