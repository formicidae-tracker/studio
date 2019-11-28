#pragma once

#include <QObject>
#include <QFuture>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

class TagExtractor : public QObject {
	Q_OBJECT
public:
	TagExtractor(QObject * parent = NULL);
	virtual ~TagExtractor();

public slots:
	void start(const fort::myrmidon::priv::TrackingDataDirectory & tdd,
	           const fs::path & basedir);
	void cancel();
signals:
	void resultReady(const std::vector<fort::myrmidon::priv::TagID> & );

private:
	std::map<fort::myrmidon::priv::TagID,uint64_t> d_tags;
	std::vector<fort::myrmidon::priv::TagID>       d_results;


	std::mutex d_mutex;
	bool       d_quit;
	QFuture<void> d_future;

};
